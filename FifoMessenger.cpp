#ifndef FIFOMESSENGER_H
#define FIFOMESSENGER_H

#include "Messenger.cpp"
#include "FifoConnector.cpp"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

class FifoMessenger : public Messenger
{
public:
    FifoMessenger(
                ENTITY ent,
                std::string s_name,
                size_t msg_size
                )
                : Messenger()
    {
        connector = new FifoConnector(ent, s_name, msg_size);
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

    message* read_get_link() override
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
                    fifoname = "/tmp/fifo" + to_string(connector->get_message()->client.c_id1);
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
            connector->set_client_process(cl);
        return connector->get_message();
    }

    void write_release_link() override
    {
        if (connector->get_entity() == SERVER)
        {
            int client_index = find_client(connector->get_client_process().c_id1);
            if (client_index == -1 || static_cast<FifoConnector*>(connector)->get_clients()[client_index].fd == -1)
            {
                cout << "Write Error" << endl;
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

#endif
