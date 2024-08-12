#ifndef MESSANGER_H
#define MESSANGER_H

#include <string>

typedef enum
{
    SERVER,
    CLIENT
} ENTITY

struct client_id
{
public:
    client_id(long id1, long id2) : c_id1(id1), c_id2(id2)
    {
    }
    long c_id1;
    long c_id2;
};

struct message
{
public:
    message(long t, client_id c, char* d) : type(t), client(c), data(d)
    {
    }

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
    void virtual open() = 0;
    void virtual close() = 0;
    ENTITY get_entity() {return entity;}
    string get_server_name() {return server_name;}
    client_id get_client() {return client;}
    size_t get_message_size() {return message_size;}
    message* get_message() {return msg;}
private:
    ENTITY entity;
    string server_name;
    client_id client;
    size_t message_size;
    message* msg;
};

class Messanger
{
public:
    Messanger(Connector con) : connector(con)
    {
    }
    void* virtual read_get_link() = 0;
    void virtual read_release_link() = 0;

    void* virtual write_get_link(client_id client) = 0;
    void virtual write_release_link() = 0;
private:
    Connector connector;
};

#endif
