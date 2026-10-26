#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>
#include <vector>

#include "db.h"
#include "authenticator.h"
#include "logging.h"

class Server
{
private:
const int port;
DB& database;
Authenticator& auth;
Logging& logger;

const std::uint32_t MAX_VECTORS = 10000;
const std::uint32_t MAX_SIZE_VECTOR = 1000;
public:
    struct sockaddr_in server_struct;
    struct sockaddr_in client_struct;

    Server(const int port, DB& database, Authenticator& auth, Logging& logger);
    void start();
    std::vector<std::uint16_t> receiveVector(int client_fd);
    void receiveExact(int client_fd, void* data, ssize_t bytes);

    void sendData(int client_fd, const void* data, ssize_t bytes);
    bool authenticate(int client_fd);
};

class Socket
{
private:
int fd;
public:
    Socket();
    Socket(const int socket_fd);

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    ~Socket();

    int get() const;
};