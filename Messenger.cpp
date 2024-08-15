#ifndef MESSENGER_H
#define MESSENGER_H

#include "Connector.cpp"

#include <string>

class Messenger
{
public:
    virtual ~Messenger()
    {
        delete connector;
    }

    void open_connection()
    {
        connector->open_connection();
    }

    void close_connection()
    {
        connector->close_connection();
    }

    virtual message* read_get_link(client_id cl) = 0;
    virtual void read_release_link() = 0;

    virtual message* write_get_link(client_id cl) = 0;
    virtual void write_release_link() = 0;
protected:
    Connector* connector;
};

#endif
