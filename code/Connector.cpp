#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <string>
#include <cstddef>
#include <cstdint>

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
    long type;
    client_id client;
    int data_size;
    uint8_t data[1];
};

class Connector
{
public:
    Connector(
        ENTITY ent,
        std::string s_name,
        size_t sz
    )
    : entity(ent)
    , server_name(s_name)
    {
        message_size = sz + offsetof(message, data);
        msg = (message*) malloc(message_size);
    }

    virtual ~Connector()
    {
        delete msg;
    }

    virtual void open_connection() = 0;

    virtual void close_connection() = 0;

    ENTITY get_entity() { return entity; }
    void set_entity(ENTITY ent) { entity = ent; }

    std::string get_server_name() { return server_name; }

    client_id get_client_process() { return client_process; }
    void set_client_process(client_id cl) { client_process = cl; }

    size_t get_message_size() { return message_size; }

    message* get_message() { return msg; }
protected:
    ENTITY entity;
    std::string server_name;
    client_id client_process;
    size_t message_size;
    message* msg;
};

#endif
