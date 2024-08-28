#ifndef MESSENGERFACTORYIMPLEMENTATION_H
#define MESSENGERFACTORYIMPLEMENTATION_H

#include "MessengerFactory.cpp"
#include "FifoMessenger.cpp"
#include "SocketMessenger.cpp"
#include "PipeMessenger.cpp"
#include "SharedMemoryMessenger.cpp"

//Factory pattern

class MessengerFactoryImplementation : public MessengerFactory
{
public:
    Messenger* make_fifo(ENTITY ent, std::string s_name, int msg_size) override
    {
        return new FifoMessenger(ent, s_name, msg_size);
    }

    Messenger* make_socket(ENTITY ent, std::string s_name, int msg_size) override
    {
        return new SocketMessenger(ent, s_name, msg_size);
    }

    Messenger* make_pipe(int msg_size) override
    {
        return new PipeMessenger(msg_size);
    }
    
    Messenger* make_shm(ENTITY ent, std::string s_name, int msg_size) override
    {
        return new SharedMemoryMessenger(ent, s_name, msg_size);
    }
};

#endif
