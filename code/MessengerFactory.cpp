#ifndef MESSENGERFACTORY_H
#define MESSENGERFACTORY_H

#include "Messenger.cpp"

#include <string>

class MessengerFactory
{
public:
    virtual Messenger* make_fifo(ENTITY ent, std::string s_name, int msg_size) = 0;
    virtual Messenger* make_socket(ENTITY ent, std::string s_name, int msg_size) = 0;
};

#endif
