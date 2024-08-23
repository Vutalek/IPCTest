#ifndef SOCKETMESSENGER_H
#define SOCKETMESSENGER_H

#include "Messenger.cpp"
#include "SocketConnector.cpp"

using namespace std;

class SocketMessenger : public Messenger
{
public:
    SocketMessenger(
        ENTITY ent,
        std::string s_name,
        size_t msg_size
    )
    : Messenger()
    {
        connector = new SocketConnector(ent, s_name, msg_size);
    }

    message* read_get_link() override
    {
        size_t nread;
        int fd;
        while(true)
        {
            if (connector->get_entity() == SERVER)
                fd = static_cast<SocketConnector*>(connector)->get_socket()->wait_server();
            else
                fd = static_cast<SocketConnector*>(connector)->get_socket()->get_server_fd();
            nread = read(
                fd,
                connector->get_message(),
                connector->get_message_size()
            );
            if (nread == 0)
            {
                if (connector->get_entity() == SERVER)
                {
                    static_cast<SocketConnector*>(connector)->get_socket()->close_fd(fd);
                    continue;
                }
            }
            else
                break;
        }
        if (connector->get_entity() == SERVER)
            connector->get_message()->client.c_id1 = fd;
        return connector->get_message();
    }

    void read_release_link() override
    {
        return;
    }

    message* write_get_link(client_id cl) override
    {
        if (connector->get_entity() == SERVER)
            connector->get_client_process() = cl;
        return connector->get_message();
    }

    void write_release_link() override
    {
        int fd;
        if (connector->get_entity() == SERVER)
            fd = connector->get_client_process().c_id1;
        else
            fd = static_cast<SocketConnector*>(connector)->get_socket()->get_server_fd();
        write(
            fd,
            connector->get_message(),
            connector->get_message_size()
        );
    }
};

#endif
