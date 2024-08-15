#ifndef FIFO_H
#define FIFO_H

#include "Messenger.cpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

#define MAX_CLIENTS 20

struct client_info
{
    int fd;
    int pid;
};

class FifoConnector : public Connector
{
public:
    FifoConnector(
                    ENTITY ent,
                    string s_name,
                    client_id cl,
                    size_t msg_size,
                    message* m
                )
                : Connector(ent, s_name, cl)
    {
        set_message(msg_size, m);
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
            string fifoname = "/tmp/fifo_" + to_string(getpid());
            unlink(fifoname.c_str());
        }
    }

    client_info* get_clients()
    {
        return clients;
    }

    int get_server_fd_r()
    {
        return server_fd_r;
    }
private:
    int server_fd_r;
    int server_fd_w;
    client_info clients[MAX_CLIENTS];
};

class FifoMessenger : public Messenger
{
public:
    FifoMessenger(
                ENTITY ent,
                std::string s_name,
                client_id cl,
                size_t msg_size,
                message* m
                )
                : Messenger()
    {
        connector = new FifoConnector(ent, s_name, cl, msg_size, m);
    }

    int find_client(pid_t pid)
    {
        int avail = -1;
        for(int i = 0; i < MAX_CLIENTS; i++)
        {
            if (static_cast<FifoConnector*>(connector)->get_clients()[i].pid == pid)
                return i;
            if (static_cast<FifoConnector*>(connector)->get_clients()[i].fd == -1 && avail == -1)
                avail = i;
        }
        if (avail != -1)
            static_cast<FifoConnector*>(connector)->get_clients()[avail].pid = pid;
        return avail;
    }

    message* read_get_link(client_id cl) override
    {
        size_t nread;
        if (connector->get_entity() == SERVER)
        {
            int client_index;
            string fifoname;
            while(true)
            {
                nread = read(
                    static_cast<FifoConnector*>(connector)->get_server_fd_r(),
                    connector->get_message(),
                    connector->get_message_size()
                );
                if (nread == 0)
                {
                    cout << "Read Error" << endl;
                    return NULL;
                }
                if (nread < offsetof(message, data))
                {
                    cout << "Read Error" << endl;
                    return NULL;
                }
                if ((client_index = find_client((pid_t) connector->get_message()->client.c_id1)) == -1)
                    continue;
                if (static_cast<FifoConnector*>(connector)->get_clients()[client_index].fd == -1)
                {
                    fifoname = "/tmp/fifo_" + to_string(connector->get_message()->client.c_id1);
                    static_cast<FifoConnector*>(connector)->get_clients()[client_index].fd = open(fifoname.c_str(), O_WRONLY);
                }
                break;
            }
        }
        else
            nread = read(
                static_cast<FifoConnector*>(connector)->get_clients()[0].fd,
                connector->get_message(),
                connector->get_message_size()
            );
        return connector->get_message();
    }

    void read_release_link() override
    {
        return;
    }

    message* write_get_link(client_id cl) override
    {
        if (connector->get_entity() == SERVER)
            connector->set_client(cl);
        return connector->get_message();
    }

    void write_release_link() override
    {
        if (connector->get_entity() == SERVER)
        {
            int client_index = find_client(connector->get_client().c_id1);
            if (client_index == -1 || static_cast<FifoConnector*>(connector)->get_clients()[client_index].fd == -1)
            {
                cout << "Ошибка записи" << endl;
                return;
            }
            write(
                static_cast<FifoConnector*>(connector)->get_clients()[client_index].fd,
                connector->get_message(),
                connector->get_message_size()
            );
        }
        else
        {
            static_cast<FifoConnector*>(connector)->get_message()->client.c_id1 = (long)getpid();
            write(
                static_cast<FifoConnector*>(connector)->get_server_fd_r(),
                connector->get_message(),
                connector->get_message_size()
            );
        }
    }
};

int main()
{


    return 0;
}

#endif
