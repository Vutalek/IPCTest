#ifndef MESSENGERFACTORYIMPLEMENTATION_H
#define MESSENGERFACTORYIMPLEMENTATION_H

#include "MessengerFactory.cpp"
#include "FifoMessenger.cpp"

class MessengerFactoryImplementation : public MessengerFactory
{
public:
    Messenger* make_fifo(ENTITY ent, std::string s_name, int msg_size) override
    {
        return new FifoMessenger(ent, s_name, msg_size);
    }
};

#endif
