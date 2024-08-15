#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <string>

typedef enum
{
    SERVER,
    CLIENT
} ENTITY;

struct client_id
{
    long c_id1;
    long c_id2;
};

struct message
{
    ~message()
    {
        delete data;
    }

    long type;
    client_id client;
    char* data;
};

class Connector
{
public:
    Connector(
        ENTITY ent,
        std::string s_name,
        size_t sz,
        message* m
    )
    : entity(ent)
    , server_name(s_name)
    , message_size(sz)
    , msg(m)
    {
    }

    virtual ~Connector()
    {
        delete msg;
    }

    virtual void open_connection() = 0;

    virtual void close_connection() = 0;

    ENTITY get_entity() { return entity; }
    std::string get_server_name() { return server_name; }
    client_id get_client() { return client; }
    void set_client(client_id cl) { client = cl; }
    size_t get_message_size() { return message_size; }
    message* get_message() { return msg; }
protected:
    ENTITY entity;
    std::string server_name;
    client_id client;
    size_t message_size;
    message* msg;
};

#endif
