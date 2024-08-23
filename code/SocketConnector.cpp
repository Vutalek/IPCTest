#ifndef SOCKETCONNECTOR_H
#define SOCKETCONNECTOR_H

#include "Connector.cpp"
#include "Socket.cpp"

#include <string>

using namespace std;

class SocketConnector : public Connector
{
public:
    SocketConnector(
        ENTITY ent,
        string s_name,
        size_t msg_size
    )
    : Connector(ent, s_name, msg_size)
    {
        socket = new Socket(s_name, ent == SERVER);
    }

    ~SocketConnector()
    {
        delete socket;
    }

    void open_connection() override
    {
        socket->open_connection();
    }

    void close_connection() override
    {
        socket->close_connection();
    }

    Socket* get_socket()
    {
        return socket;
    }
private:
    Socket* socket;
};

#endif
