#include "Messanger.cpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>

class PipeMessanger : public Messanger
{
public:
    PipeMessanger(Connector con) : Messanger(con)
    {
        if (pipe(pipe_pfd) == -1)
        {
            exit(1);
        }
    }

    ~PipeMessanger()
    {
        close(pipe_pfd[0]);
        close(pipe_pfd[1]);
    }

    message* read_get_link(client_id cl) override
    {
        return read(fd[0], data, DATA_SIZE
    }

    void read_release_link() override
    {
        //
    }

    message* write_get_link(client_id cl) override
    {
        //return (void*)&pipe_pfd[1];
    }

    void write_release_link() override
    {
        //write(fd[1], data, DATA_SIZE);
    }

private:
    int pipe_pfd[2];
};

