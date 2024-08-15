#ifndef MESSENGER_H
#define MESSENGER_H

#include <string>

typedef enum
{
    SERVER,
    CLIENT
} ENTITY;

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
              client_id cl
              )
              : entity(ent)
              , server_name(s_name)
              , client(cl)
    {
    }

    virtual ~Connector()
    {
        delete msg;
    }

    virtual void open_connection() = 0;

    void set_message(size_t sz, message* m)
    {
        message_size = sz;
        msg = m;
    }

    virtual void close_connection() = 0;

    ENTITY get_entity() {return entity;}
    std::string get_server_name() {return server_name;}
    client_id get_client() {return client;}
    void set_client(client_id cl) {client = cl;}
    size_t get_message_size() {return message_size;}
    message* get_message() {return msg;}
protected:
    ENTITY entity;
    std::string server_name;
    client_id client;
    size_t message_size;
    message* msg;
};

class Messenger
{
public:
    virtual ~Messenger()
    {
        delete connector;
    }

    void open_connector()
    {
        connector->open_connection();
    }

    void close_connector()
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

/* Общая структура приложения
int main()
{
    MessengerFactory fac = MessengerFactory();
    Messenger* messenger = fac.fifo(...);
    messenger->open();
    int t1 = time();
    char* data_block = messenger->read_get_link()->data;
    std::cout << data_block << std::endl;
    int t2 = time();
    messenger->read_release_link();
    messenger->close();
    std::cout << t2 - t1 << std::endl;
    return 0;
}
*/
