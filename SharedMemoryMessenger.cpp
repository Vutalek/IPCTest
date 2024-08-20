#ifndef FIFOMESSENGER_H
#define FIFOMESSENGER_H

#include "Messenger.cpp"
#include "SharedMemoryConnector.cpp"

using namespace std;

class SharedMemoryMessenger : public Messenger
{
public:
    SharedMemoryMessenger(ENTITY ent,
                std::string s_name,
                size_t msg_size,
                message* m
                )
                : Messenger()
                {
                    connector = new SharedMemoryConnector(ent, s_name, msg_size, m);
                }


    client *get_client(SMIQ_PSHM *p, pid_t pid)
    {
        int i, avail = - 1;
        char shmname [SERVER_NAME_MAX + 50];
        for (i= 0; i < MAX_CLIENTS; i++) {
            if (p->sq_clients[i].cl_pid == pid)
                return &p->sq_clients[i];
            if (p->sq_clients[i].cl_pid == 0 && avail == -1)
                avail = i;
        }
        if (avail == -1) {
             errno = EADDRNOTAVAIL;
        }
        p->sq_clients[avail].cl_pid = pid;
        /*connector->mkshm_name_client(pid, shmname, sizeof (shmname));*/
        p->sq_clients[avail].cl_fd = shm_open(shmname, O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);
        p->sq_clients[avail].cl_mem = (shared_mem *)mmap(NULL, MEM_SIZE(p->sq_msgsize), PROT_READ | PROT_WRITE,
                                            MAP_SHARED, p->sq_clients[avail].cl_fd, 0);
        p->sq_clients[avail].cl_mem, MAP_FAILED;
        return &p->sq_clients[avail];
    }

    message* read_get_link(client_id client) override
    {
    	struct client *cp;
        shared_mem *sm;
        if (p->entity == SERVER) {
            p->sq_client = *client;
            cp = get_client(p, client->c_id1);
            sm = cp->cl_mem;
        }
        else
            sm = p->sq_srv_mem;
        op_semi(sm, SEMI_WRITE, SEMI_WAIT);
        if (p->entity == CLIENT)
            sm->sm_msg.client.c_id1 = getpid();                      //?
        return &sm->sm_msg;
    }

    void read_release_link() override
    {
        client *cp;
        shared_mem *sm;
        if (connector.p->entity == SERVER) {
            cp = get_client(p, p->sq_client.c_id1);
            sm = cp->cl_mem;
        }
        else
            sm = p->sq_srv_mem;
        op_semi(sm, SEMI_READ, SEMI_POST);
    }

    message* write_get_link(client_id client) override
    {
        struct shared_mem *sm;
        if (connector->p->entity == SERVER)
            sm = p->sq_srv_mem;
        else
            sm = p->sq_clients[0].cl_mem;
        op_semi(sm, SEMI_READ, SEMI_WAIT);
        return &sm->sm_msg;
    }

    void write_release_link() override
    {
    	struct shared_mem *sm;
        if (p->entity == SERVER)
            sm = p->sq_srv_mem;
        else
            sm = p->sq_clients[0].cl_mem;
        op_semi(sm, SEMI_WRITE, SEMI_POST);
    }

    void op_semi(shared_mem *m, int sem_num, int sem_op)
    {
        sem_t *sem_p= NULL;
        if (sem_num == SEMI_WRITE)
            sem_p = &m->sm_sem_w;
        else
            sem_p = &m->sm_sem_r;
        switch (sem_op) {
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
