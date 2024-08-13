#ifndef MESSANGER_H
#define MESSANGER_H

#include <string>

typedef enum
{
    SERVER,
    CLIENT
}; ENTITY

struct client_id
{
    client_id(
              long id1,
              long id2
              )
              : c_id1(id1)
              , c_id2(id2)
    {
    }
    long c_id1;
    long c_id2;
};

struct message
{
    message(
            long t,
            client_id c,
            char* d
            )
            : type(t)
            , client(c)
            , data(d)
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
    Connector(
              ENTITY ent,
              std::string s_name,
              client_id cl,
              size_t m_size,
              message* m
              )
              : entity(ent)
              , server_name(s_name)
              , client(cl)
              , message_size(m_size)
              , msg(m)
    {
    }

    virtual void open() = 0;
    virtual void close() = 0;

    ENTITY get_entity() {return entity;}
    std::string get_server_name() {return server_name;}
    client_id get_client() {return client;}
    size_t get_message_size() {return message_size;}
    message* get_message() {return msg;}
protected:
    ENTITY entity;
    std::string server_name;
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
    virtual void* read_get_link() = 0;
    virtual void read_release_link() = 0;

    virtual void* write_get_link(client_id client) = 0;
    virtual void write_release_link() = 0;
protected:
    Connector connector;
};

#endif
