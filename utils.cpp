#include "utils.h"

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <thread>
#include <vector>

int createListenPort(const char *address, int port, TransportType type)
{
    int socket_fd;
    int opt = 1;
    struct sockaddr_in addrInfo;
    if (type != TT_TCP && type != TT_UDP)
    {
        std::cerr << "Unknown transport type: " << type << std::endl;
        return -1;
    }
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        std::cerr << "Socket fd could not be created" << std::endl;
        return -1;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        std::cerr << "Setsocket error" << std::endl;
        return -1;
    }
    addrInfo.sin_family = AF_INET;
    addrInfo.sin_addr.s_addr = inet_addr(address);
    addrInfo.sin_port = htons(port);
    if (bind(socket_fd, (struct sockaddr *)&addrInfo, sizeof(addrInfo)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }
    return socket_fd;
}

void echoClientHandler(int socket_remote, sockaddr_in addr, socklen_t addr_len)
{
    char buffer[MAX_CLIENT_DATA_SIZE];
    pthread_cleanup_push([](void *arg)
                         { int socket_remote = *(int*)arg;
                         printf("Closing socket %d\n", socket_remote);
                         close(socket_remote); },
                         (void *)&socket_remote);
    while (1)
    {
        int readCount = read(socket_remote, &buffer, 10);
        if (readCount == -1)
        {
            // Error
            std::cerr << "Read error!" << std::endl;
            perror("read");
            break;
        }
        else if (readCount == 0)
        {
            // EOF
            std::cerr << "EOF received" << std::endl;
            break;
        }
        else
        {
            std::cout << "Read: '" << buffer << "' " << getpid() << std::endl;
            std::cout << "Address: " << inet_ntoa(addr.sin_addr) << std::endl;
        }
    }
    // Clean up and exit
    pthread_cleanup_pop(1);
    return;
}

bool handleListener(int socket_fd)
{
    /**
     * @brief
     * TODO Remove exited threads from the vector?
     */
    std::vector<std::thread> threads;
    while (1)
    {
        if (listen(socket_fd, MAX_QUEUED_CONN) < 0)
        {
            std::cerr << "Listen failed!" << std::endl;
            close(socket_fd);
            return false;
        }
        else
        {
            sockaddr_in remote_socket;
            socklen_t remove_socket_len = sizeof(remote_socket);
            int socket_remote = accept(socket_fd, (sockaddr *)&remote_socket, &remove_socket_len);
            if (socket_remote == -1)
            {
                std::cerr << "Accept had error!" << std::endl;
                perror("accept");
                return false;
            }
            threads.push_back(std::thread(&echoClientHandler, socket_remote, remote_socket, remove_socket_len));
        }
    }
    return true;
}