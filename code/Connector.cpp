#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <string>
#include <cstddef>
#include <cstdint>

//message.type codes
//Message carry byte data
#define M_TRANSMISSION_DATA 0

//Message is a string of words
#define M_MESSAGE 1

//Special message that indicates that client ended transmitting data
#define M_TRANSMISSION_END 900

//Entity of application
typedef enum
{
    SERVER,
    CLIENT
} ENTITY;

//Structure that holds information describing client
struct client_id
{
    long c_id1;
    long c_id2;
};

//Message structure
struct message
{
    long type;
    client_id client;
    int data_size;
    uint8_t data[1];
};

//Connector class can open and close connection between processes
//also it carries message buffer msg
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
