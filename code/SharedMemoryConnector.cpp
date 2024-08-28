#ifndef SHAREDMEMORYCONNECTOR_H
#define SHAREDMEMORYCONNECTOR_H

#include "Connector.cpp"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define MEM_SIZE(s) (sizeof(shared_mem) - sizeof(message) + (s))

#define SEMI_READ 0
#define SEMI_WRITE 1
#define SEMI_DESTROY 2
#define SEMI_POST 1
#define SEMI_WAIT -1

#define MAX_CLIENTS 20

using namespace std;

struct shared_mem {
    sem_t sem_w;
    sem_t sem_r;
    message sm_msg;
};

struct client {
        pid_t cl_pid; 
        int cl_fd;
        shared_mem *cl_mem;
};

class SharedMemoryConnector : public Connector
{
public:
    SharedMemoryConnector(
                 ENTITY ent,
                 string s_name,
                 size_t msg_size
                 )
                 : Connector(ent, s_name, msg_size)
    {
    }

    void open_connection() override
    {
        string shmname = "tmpshm_" + server_name;
        if (entity == SERVER) 
        {
            if ((srv_fd = shm_open(shmname.c_str(), O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) != -1) 
            {
                shm_unlink(shmname.c_str());
                close(srv_fd);
            }
            srv_fd = shm_open(shmname.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            ftruncate(srv_fd, MEM_SIZE(message_size));
            srv_mem = (shared_mem *)mmap(NULL, MEM_SIZE(message_size), PROT_READ | PROT_WRITE, MAP_SHARED, srv_fd, 0); 
           
            sem_init(&srv_mem->sem_w, true, 1);
            sem_init(&srv_mem->sem_r, true, 0);
        }
        else 
        {
            srv_fd = shm_open(shmname.c_str(), O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            srv_mem = (shared_mem *)mmap(NULL, MEM_SIZE(message_size), PROT_READ | PROT_WRITE, MAP_SHARED, srv_fd, 0);
            
            shmname = "tmpshm" + to_string(getpid());
            
            if ((clients[0].cl_fd = shm_open(shmname.c_str(), O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))!=-1) 
            {
                shm_unlink(shmname.c_str());
                close(clients[0].cl_fd);
            }
            clients[0].cl_fd= shm_open(shmname.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            ftruncate(clients[0].cl_fd, MEM_SIZE(message_size));
            clients[0].cl_mem = (shared_mem *)mmap(NULL, MEM_SIZE(message_size), PROT_READ | PROT_WRITE, MAP_SHARED, clients[0].cl_fd, 0);
                                        
            sem_init(&clients[0].cl_mem->sem_w, true, 1);
            sem_init(&clients[0].cl_mem->sem_r, true, 0);
        }
    }

    void close_connection() override
    {
        string shmname = "tmpshm_" + server_name;
        
	if (entity == SERVER) 
	{
	    munmap(srv_mem, MEM_SIZE(message_size));
	    shm_unlink(shmname.c_str());
	}
	else 
	{
	    munmap(srv_mem, MEM_SIZE(message_size));
	    shm_unlink(shmname.c_str());
	    
	    shmname = "tmpshm" + to_string(getpid());
	    
	    munmap(clients[0].cl_mem, MEM_SIZE(message_size));
	    shm_unlink(shmname.c_str());
	}
    }
    
    shared_mem* get_srv_mem(){ return srv_mem; }
    
    client* get_clients(){ return clients; }    
private:
    int srv_fd; 
    shared_mem *srv_mem;
    client clients[MAX_CLIENTS];
};

#endif
