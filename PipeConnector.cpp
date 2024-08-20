#ifndef PIPECONNECTOR_H
#define PIPECONNECTOR_H

#include "Connector.cpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>

using namespace std;

#define MAX_CLIENTS 20


struct client_info
{
    int fd;
    int pid;
};

class PipeConnector : public Connector
{
public:
    PipeConnector(
        ENTITY ent,
        string s_name,
        size_t msg_size,
        message* m
    )
    : Connector(ent, s_name, msg_size, m)
    {
    }

    void init_clients()
    {
        for(int i = 0; i < MAX_CLIENTS; i++)
            clients[i].fd = -1;
    }

    void close_clients()
    {
        for(int i = 0; i < MAX_CLIENTS; i++)
            if (clients[i].fd != -1)
            {
                close(clients[i].fd);
                clients[i].fd = -1;
            }
    }

    void open_connection() override
    {
        if (entity == SERVER)
        {
            init_clients();
            pipe(pfd);
            
            
            close(fd[1]);
     	    read(fd[0], msg, message_size);
            close(fd[0]);
        }
        else
        {   
       	    pipe(pfd);
       	    
       	    
            close(fd[0]);
            write(fd[1], msg, message_size);
            close(fd[1]);
        }
    }

    void close_connection() override
    {
    	close_clients;
	exit(0);
    }
    
private:
    int pfd[2];
};

#endif
