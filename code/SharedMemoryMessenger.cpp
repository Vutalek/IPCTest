#ifndef SHAREDMEMORYMESSENGER_H
#define SHAREDMEMORYMESSENGER_H

#include "Messenger.cpp"
#include "SharedMemoryConnector.cpp"

using namespace std;

class SharedMemoryMessenger : public Messenger
{
public:
    SharedMemoryMessenger(ENTITY ent,
                string s_name,
                size_t msg_size
                )
                : Messenger()
                {
                    connector = new SharedMemoryConnector(ent, s_name, msg_size);
                }


    client *get_client(pid_t pid)
    {
        int i, avail = - 1;
        string shmname = "tmpshm" + to_string(pid);
        for (i= 0; i < MAX_CLIENTS; i++) 
        {
            if (static_cast<SharedMemoryConnector*>(connector)->get_clients()[i].cl_pid == pid)
                return &static_cast<SharedMemoryConnector*>(connector)->get_clients()[i];
            if (static_cast<SharedMemoryConnector*>(connector)->get_clients()[i].cl_pid == 0 && avail == -1)
                avail = i;
        }
        static_cast<SharedMemoryConnector*>(connector)->get_clients()[avail].cl_pid = pid;   
        static_cast<SharedMemoryConnector*>(connector)->get_clients()[avail].cl_fd = shm_open(shmname.c_str(), O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        static_cast<SharedMemoryConnector*>(connector)->get_clients()[avail].cl_mem = (shared_mem *)mmap(NULL, MEM_SIZE(connector->get_message_size()), PROT_READ | PROT_WRITE, MAP_SHARED, static_cast<SharedMemoryConnector*>(connector)->get_clients()[avail].cl_fd, 0);
        return &static_cast<SharedMemoryConnector*>(connector)->get_clients()[avail];
    }

    message* read_get_link() override
    {
        shared_mem *sm;
        
        if (connector->get_entity() == SERVER) 
            sm = static_cast<SharedMemoryConnector*>(connector)->get_srv_mem();
        else 
            sm = static_cast<SharedMemoryConnector*>(connector)->get_clients()[0].cl_mem;
                   
        op_semi(sm, SEMI_READ, SEMI_WAIT);
       
        return &sm->sm_msg;
    }

    void write_release_link() override
    {
        client *cp;
        shared_mem *sm;
        if (connector->get_entity() == SERVER) 
        {
            cp = get_client(connector->get_client_process().c_id1);
            sm = cp->cl_mem;
        }
        else
            sm = static_cast<SharedMemoryConnector*>(connector)->get_srv_mem();
        op_semi(sm, SEMI_READ, SEMI_POST);
    }

    message* write_get_link(client_id cl) override
    {
        client *cp;
        shared_mem *sm;
        if (connector->get_entity() == SERVER) 
        {
            connector->set_client_process(cl);   
            cp = get_client(cl.c_id1);
            sm = cp->cl_mem;
        }
        else
            sm = static_cast<SharedMemoryConnector*>(connector)->get_srv_mem();
            
        op_semi(sm, SEMI_WRITE, SEMI_WAIT);
        
        if (connector->get_entity() == CLIENT)
            sm->sm_msg.client.c_id1 = getpid(); 
                        
        return &sm->sm_msg;
    }

    void read_release_link() override
    {
    	shared_mem *sm;
        if (connector->get_entity() == SERVER)
            sm = static_cast<SharedMemoryConnector*>(connector)->get_srv_mem();
        else
            sm = static_cast<SharedMemoryConnector*>(connector)->get_clients()[0].cl_mem;
        op_semi(sm, SEMI_WRITE, SEMI_POST);
    }

    void op_semi(shared_mem *m, int sem_num, int sem_op)
    {
        sem_t *sem_p = NULL;
        
        if (sem_num == SEMI_WRITE) 
            sem_p = &m->sem_w;
        else 
            sem_p = &m->sem_r;
        switch (sem_op) 
        {
            case SEMI_WAIT:
                sem_wait(sem_p);
                break;
            case SEMI_POST:
                sem_post(sem_p);
                break;
            case SEMI_DESTROY:
                sem_destroy(sem_p);
                break;
        }
    }
};

#endif
