#ifndef IPCTEST_H
#define IPCTEST_H

#include "Application.cpp"
#include "FifoMessenger.cpp"

#include <iostream>

using namespace std;

#define SERVER_NAME "ipc_server"
#define DATA_BLOCK 512

class IPCTest : public Application
{
public:
    IPCTest(
        string entity,
        string ipc_method,
        string file
    )
    : Application()
    , method(ipc_method)
    , path_to_file(file)
    {
        if (entity == "server")
            application_entity = SERVER;
        else
            application_entity = CLIENT;
    }

    ~IPCTest()
    {
        delete messenger;
    }

    void init() override
    {
        if (method == "fifo")
        {
            messenger = new FifoMessenger(
                application_entity,
                SERVER_NAME,
                DATA_BLOCK,
                new message()
            );
            messenger->open_connection();
            if (application_entity == SERVER)
                cout << "Found Client. Connection established." << endl;
            else
                cout << "Connection established." << endl;

        }
    }

    bool done() override
    {
        return true;
    }

    void action() override
    {

    }

    void exit() override
    {
        messenger->close_connection();
        cout << "Connection closed." << endl;
    }
private:
    ENTITY application_entity;
    string method;
    string path_to_file;

    Messenger* messenger;
};

#endif
