#ifndef MESSENGERFACTORY_H
#define MESSENGERFACTORY_H

#include "Connector.cpp"
#include "Messenger.cpp"

#include <string>

class MessengerFactory
{
public:
    virtual Messenger* make_fifo(ENTITY ent, std::string s_name, int msg_size) = 0;
};

#endif
