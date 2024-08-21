#ifndef FIFOCONNECTOR_H
#define FIFOCONNECTOR_H

#include "Connector.cpp"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

#define MAX_CLIENTS 20

struct fifo_client
{
    int fd;
    pid_t pid;
};

class FifoConnector : public Connector
{
public:
    FifoConnector(
        ENTITY ent,
        string s_name,
        size_t msg_size
    )
    : Connector(ent, s_name, msg_size)
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
        string fifoname = "/tmp/fifo_" + server_name;
        if (entity == SERVER)
        {
            init_clients();

            mkfifo(fifoname.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

            server_fd_r = open(fifoname.c_str(), O_RDONLY);

            server_fd_w = open(fifoname.c_str(), O_WRONLY);
        }
        else
        {
            server_fd_r = open(fifoname.c_str(), O_WRONLY);

            fifoname = "/tmp/fifo" + to_string(getpid());
            unlink(fifoname.c_str());
            mkfifo(fifoname.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

            clients[0].fd = open(fifoname.c_str(), O_RDONLY | O_NONBLOCK);
            int flags = fcntl(clients[0].fd, F_GETFL);
            flags &= ~O_NONBLOCK;
            fcntl(clients[0].fd, F_SETFL, flags);

            clients[1].fd = open(fifoname.c_str(), O_WRONLY);
        }
    }

    void close_connection() override
    {
        close_clients();
        close(server_fd_r);
        if (entity == SERVER)
            close(server_fd_w);
        else
        {
            string fifoname = "/tmp/fifo" + to_string(getpid());
            unlink(fifoname.c_str());
        }
    }

    int get_server_fd_r() { return server_fd_r; }

    fifo_client* get_clients() { return clients; }
private:
    int server_fd_r;
    int server_fd_w;

    fifo_client clients[MAX_CLIENTS];
};

#endif
