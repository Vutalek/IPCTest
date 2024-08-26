#ifndef PIPEMESSENGER_H
#define PIPEMESSENGER_H

#include "Messenger.cpp"
#include "PipeConnector.cpp"

#include <cstddef>
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

class PipeMessenger : public Messenger
{
public:
    PipeMessenger(
        size_t msg_size
    )
    : Messenger()
    {
        connector = new PipeConnector(msg_size);
    }

    message* read_get_link() override
    {
        size_t nread;
        if (connector->get_entity() == SERVER)
        {
            nread = read(
                static_cast<PipeConnector*>(connector)->get_server_fd_r(),
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
        }
        else
            nread = read(
                static_cast<PipeConnector*>(connector)->get_client_fd_r(),
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
            if (static_cast<PipeConnector*>(connector)->get_client_fd_w() == -1)
            {
                cout << "Write Error" << endl;
                return;
            }
            write(
                static_cast<PipeConnector*>(connector)->get_client_fd_w(),
                connector->get_message(),
                connector->get_message_size()
            );
        }
        else
        {
            connector->get_message()->client.c_id1 = (long)getpid();
            write(
                static_cast<PipeConnector*>(connector)->get_server_fd_w(),
                connector->get_message(),
                connector->get_message_size()
            );
        }
    }
};

#endif
