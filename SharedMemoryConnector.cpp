#ifndef SHAREDMEMORYCONNECTOR_H
#define SHAREDMEMORYCONNECTOR_H

#include "Connector.cpp"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <semaphore.h>
#include <cstdlib>

#define MEM_SIZE(s)\
    (sizeof(struct shared_mem) - sizeof(struct message) + (s))
#define SEMI_READ 0
#define SEMI_WRITE 1
#define SEMI_DESTROY 2
#define SEMI_POST 1
#define SEMI_WAIT -1
#define SERVER_NAME_MAX 30
#define MAX_CLIENTS 50
using namespace std;


struct shared_mem {
    sem_t sm_sem_w;
    sem_t sm_sem_r;
    message sm_msg;
};

struct client {
        pid_t cl_pid; // идентификатор процесса клиента
        int cl_fd; // дескриптор файла разделяемой памяти клиента
        shared_mem *cl_mem; // указатель на сегмент памяти клиента
};

typedef struct {
    ENTITY entity; //сущность
    char sq_name[SERVER_NAME_MAX]; // имя сервера
    int sq_srv_fd; // дескриптор файла разделяемой памяти сервера
    shared_mem *sq_srv_mem; // указатель на сегмент памяти сервера
    client sq_clients[MAX_CLIENTS]; // клиент использует только [0]
    client_id sq_client; // идентификатор клиента (для сервера)
    size_t sq_msgsize; // размер сообщения
}SMIQ_PSHM;


class SharedMemoryConnector : public Connector
{
public:
    SharedMemoryConnector(
                        ENTITY ent,
                        string s_name,
                        size_t msg_size,
                        message* m
                        )
                        : Connector(ent, s_name, msg_size, m)
    {
    }

    void mkshm_name_server(const SMIQ_PSHM *p, char *shmname, size_t shmname_max) {
        snprintf(shmname, shmname_max, "/smipshm-%s", p->sq_name);
    }

    void mkshm_name_client(pid_t pid, char *shmname, size_t shmname_max) {
        snprintf(shmname, shmname_max, "/smipshm-%d", pid);
    }


    void open_connection() override
    {
        char shmname[SERVER_NAME_MAX + 50];
        p = (SMIQ_PSHM *)calloc(1, sizeof(SMIQ_PSHM));
        p->entity = entity;
        p->sq_msgsize = message_size;
        if (server_name.length() >= SERVER_NAME_MAX) {
            errno= ENAMETOOLONG;
        }
        strcpy(p->sq_name, server_name.c_str());
        mkshm_name_server(p, shmname, sizeof(shmname));
        if (p->entity == SERVER) {
            if ((p->sq_srv_fd = shm_open(shmname, O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR)) != -1) {
                (void)shm_unlink(shmname);
                (void)close(p->sq_srv_fd);
            }
            p->sq_srv_fd = shm_open(shmname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
            ftruncate(p->sq_srv_fd, MEM_SIZE(message_size));
            p->sq_srv_mem = (shared_mem *)mmap(NULL, MEM_SIZE(message_size), PROT_READ | PROT_WRITE,
                                MAP_SHARED, p->sq_srv_fd, 0);
            p->sq_srv_mem, MAP_FAILED;                                                      //??
            sem_init(&p->sq_srv_mem->sm_sem_w, true, 1);
            sem_init(&p->sq_srv_mem->sm_sem_r, true, 0);
        }
        else {
            p->sq_srv_fd = shm_open(shmname, O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);
            p->sq_srv_mem = (shared_mem *)mmap(NULL, MEM_SIZE(message_size),
                                PROT_READ | PROT_WRITE, MAP_SHARED, p->sq_srv_fd, 0);
            p->sq_srv_mem, MAP_FAILED;                                                      //?
            mkshm_name_client(getpid(), shmname, sizeof(shmname));
            if ((p->sq_clients[0].cl_fd = shm_open(shmname, O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR))!=-1) {
                (void)shm_unlink(shmname);
                (void)close(p->sq_clients[0].cl_fd);
            }
            p->sq_clients[0].cl_fd= shm_open(shmname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
            ftruncate(p->sq_clients[0].cl_fd, MEM_SIZE(message_size));
            p->sq_clients[0].cl_mem = (shared_mem *)mmap(NULL, MEM_SIZE(message_size), PROT_READ | PROT_WRITE,
                                        MAP_SHARED, p->sq_clients[0].cl_fd, 0);
            p->sq_clients[0].cl_mem, MAP_FAILED;                                            //?
            sem_init(&p->sq_clients[0].cl_mem->sm_sem_w, true, 1);
            sem_init(&p->sq_clients[0].cl_mem->sm_sem_r, true, 0);
        }
    }

    void close_connection() override
    {
	if (p->entity == SERVER) {
	
	}
	else {
	
	}
    }

    SMIQ_PSHM *p;
};

#endif
