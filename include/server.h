#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>
#include <vector>

class server
{
private:
const int port;
std::vector<std::vector<std::uint16_t>> data;
const std::uint32_t MAX_VECTORS = 10000;
const std::uint32_t MAX_SIZE_VECTOR = 1000;
public:
    struct sockaddr_in server_struct;
    struct sockaddr_in client_struct;
    socklen_t client_len = sizeof(client_struct);

    server(const int port) : port(port) {};
    void start();
    void receiveData(int client_fd);
    void receiveExact(int client_fd, void* data, ssize_t bytes);
    void sendData(int client__fd, void* data, ssize_t bytes);
};