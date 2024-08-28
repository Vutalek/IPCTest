#ifndef PIPECONNECTOR_H
#define PIPECONNECTOR_H

#include "Connector.cpp"

#include <sys/wait.h>
#include <unistd.h>
#include <string>

using namespace std;

class PipeConnector : public Connector
{
public:
    PipeConnector(
        size_t msg_size
    )
    : Connector(SERVER, "pipe", msg_size)
    {
    }

    void close_fd()
    {
        for(int i = 0; i < 2; i++)
        {
            close(server_fd[i]);
            close(client_fd[i]);
        }
    }

    void open_connection() override
    {
        pipe(server_fd);
        pipe(client_fd);

        pid = fork();

        if (pid == 0)
            set_entity(CLIENT);
    }

    void close_connection() override
    {
        close_fd();
        if (entity == SERVER)
            waitpid(pid, NULL, 0);
    }

    int get_server_fd_r() { return server_fd[0]; }
    int get_server_fd_w() { return server_fd[1]; }

    int get_client_fd_r() { return client_fd[0]; }
    int get_client_fd_w() { return client_fd[1]; }
private:
    pid_t pid;

    int server_fd[2];
    int client_fd[2];
};

#endif
