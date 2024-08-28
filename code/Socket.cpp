#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <unistd.h>

#define MAXCONN 20

using namespace std;

//Socket class is a wrap around standart C functions
class Socket
{
public:
    Socket(
        string name,
        bool server
    )
    : is_server(server)
    , server_name(name)
    {
        if (is_server)
            unlink(server_name.c_str());
        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        set_maximum_fd(fd);
    }

    void open_connection()
    {
        sockaddr_un sock = make_sockaddr_un(server_name);
        socklen_t sock_len = sizeof(sock);
        if (is_server)
        {
            FD_ZERO(&set);
            bind(fd, (sockaddr*)&sock, sock_len);
            listen(fd, MAXCONN);
            FD_SET(fd, &set);
        }
        else
            connect(fd, (sockaddr*)&sock, sock_len);
    }

    sockaddr_un make_sockaddr_un(string name)
    {
        sockaddr_un sock;
        strcpy(sock.sun_path, name.c_str());
        sock.sun_family = AF_UNIX;
        return sock;
    }

    int wait_server()
    {
        if (is_server)
        {
            fd_set fd_set_read;
            int client_fd;
            sockaddr_un from;
            socklen_t from_len = sizeof(from);

            while(true)
            {
                fd_set_read = set;
                select(maximum_fd + 1, &fd_set_read, NULL, NULL, NULL);
                for(int i = 0; i <= maximum_fd; i++)
                    if (FD_ISSET(i, &fd_set_read))
                    {
                        if (i == fd)
                        {
                            client_fd = accept(fd, (sockaddr*)&from, &from_len);
                            FD_SET(client_fd, &set);
                            set_maximum_fd(client_fd);
                            continue;
                        }
                        else
                            return i;
                    }
            }
        }
        else
            return -1;
    }

    int get_server_fd()
    {
        return fd;
    }

    void close_connection()
    {
        if (is_server)
        {
            for(int i = 0; i <= maximum_fd; i++)
                if (FD_ISSET(i, &set))
                    close(i);
            unlink(server_name.c_str());
        }
        else
            close(fd);
    }

    void close_fd(int f)
    {
        close(f);
        FD_CLR(f, &set);
        reset_maximum_fd(f);
    }

    void set_maximum_fd(int f)
    {
        if (f > maximum_fd)
            maximum_fd = f;
    }

    void reset_maximum_fd(int f)
    {
        if (f == maximum_fd)
            maximum_fd--;
    }
private:
    bool is_server;
    int fd;
    string server_name;

    fd_set set;
    int maximum_fd;
};

#endif
