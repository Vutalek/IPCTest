#ifndef MESSENGER_H
#define MESSENGER_H

#include "Connector.cpp"

//Messenger class can get links to message from connector
//to perform data reading/writing
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

    virtual message* read_get_link() = 0;
    virtual void read_release_link() = 0;

    virtual message* write_get_link(client_id cl) = 0;
    virtual void write_release_link() = 0;

    ENTITY get_messenger_entity() { return connector->get_entity(); }
protected:
    Connector* connector;
};

#endif
