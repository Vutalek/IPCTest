#ifndef IPCTEST_H
#define IPCTEST_H

#include "Application.cpp"
#include "MessengerFactoryImplementation.cpp"

#include <iostream>
#include <cstring>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

using namespace std;

#define SERVER_NAME "ipc_server"
#define DATA_BLOCK 512

#define DEBUG_MODE 0

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
        else if (entity == "client")
            application_entity = CLIENT;
        else
        {
            cout << "Incorrect role." << endl;
            ended = true;
        }

        factory = MessengerFactoryImplementation();
    }

    ~IPCTest()
    {
        delete messenger;
    }

    void init() override
    {
        if (method == "fifo")
            messenger = factory.make_fifo(
                application_entity,
                SERVER_NAME,
                DATA_BLOCK
            );
        else
        {
            cout << "Incorrect method." << endl;
            ended = true;
            return;
        }

        messenger->open_connection();
        cout << "Connection established." << endl;
        if (application_entity == SERVER)
        {
            out_file_fd = creat(path_to_file.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (out_file_fd == -1)
            {
                cout << "Error in opening file" << endl;
                ended = true;
                return;
            }
        }
        else
        {
            in_file_fd = open(path_to_file.c_str(), O_RDONLY);
            if (in_file_fd == -1)
            {
                cout << "Error in opening file" << endl;
                ended = true;
                return;
            }
        }
    }

    bool done() override
    {
        return ended;
    }

    void action() override
    {
        if (application_entity == SERVER)
        {
            message* message_in = messenger->read_get_link();

            write(out_file_fd, message_in->data, message_in->data_size);

            messenger->read_release_link();
#if DEBUG_MODE
            message* message_out = messenger->write_get_link(message_in->client);

            char char_getted[100];
            sprintf(char_getted, "Server received %d bytes.\n", message_in->data_size);
            uint8_t* getted = char_to_byte(char_getted);
            memcpy(message_out->data, getted, strlen(char_getted) + 1);
            delete[] getted;

            messenger->write_release_link();
#endif
        }
        else
        {
            message* msg;
            uint8_t buffer[DATA_BLOCK];
            int nread = 0;
            timeval t1, t2;
            unsigned long long file_size = 0;
            gettimeofday(&t1, NULL);
            while((nread = read(in_file_fd, buffer, DATA_BLOCK)) != 0)
            {
                file_size += nread;
                msg = messenger->write_get_link(client_id());
                msg->data_size = nread;
                memcpy(msg->data, buffer, nread);
                messenger->write_release_link();
#if DEBUG_MODE
                cout << "Send " << nread << " bytes." << endl;
                msg = messenger->read_get_link();
                cout << msg->data << endl;
                messenger->read_release_link();
#endif
            }
            gettimeofday(&t2, NULL);
            double sec = t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1000000.0;
            cout << "File size: " << file_size << " Bytes" << endl;
            cout << "General time: " << sec << " sec"<< endl;
            cout << "Bandwidth: " << (file_size/sec)/1000000 << " MBytes/sec" << endl;
            ended = true;
        }
    }

    void exit() override
    {
        messenger->close_connection();
        cout << "Connection closed." << endl;
        close(in_file_fd);
        close(out_file_fd);
    }

    uint8_t* char_to_byte(char* str)
    {
        size_t len = strlen(str) + 1;
        uint8_t* byte_str = new uint8_t[len];
        for(size_t i = 0; i < len; i++)
            byte_str[i] = (uint8_t)str[i];
        return byte_str;
    }


private:
    ENTITY application_entity;
    string method;
    string path_to_file;

    MessengerFactoryImplementation factory;
    Messenger* messenger;
    int in_file_fd;
    int out_file_fd;

    bool ended = false;
};

#endif
