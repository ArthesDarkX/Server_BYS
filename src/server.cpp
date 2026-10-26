#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <cerrno>

#include "db.h"
#include "authenticator.h"
#include "server.h"
#include "calculator.h"

Server::Server(const int port, DB& database, Authenticator& auth, Logging& logger)
: port(port), database(database), auth(auth), logger(logger) {};

Socket::Socket()
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        throw std::runtime_error(std::string("Ошибка при создании сокета: ") + std::strerror(errno));
    }
}
Socket::~Socket()
{
    close(fd);
}
int Socket::get() const
{
    return fd;
}

Socket::Socket(const int socket_fd) : fd(socket_fd) {};

void Server::start()
{
    try
    {
        Socket server_socket;
        int server_fd = server_socket.get();

        memset(&server_struct, 0, sizeof(server_struct));
        server_struct.sin_addr.s_addr = INADDR_ANY;
        server_struct.sin_family = AF_INET;
        server_struct.sin_port = htons(port);
        
        if (bind(server_fd, (sockaddr*)&server_struct, sizeof(server_struct)) == -1)
        {
            throw std::runtime_error(std::string("Ошибка при присоединении порта: ") + std::strerror(errno));
        }

        logger.write(Logging::Info, "Сервер запущен на порту " + std::to_string(port));

        if ((listen(server_fd, 1)) == -1)
        {
            throw std::runtime_error(std::string("Ошибка при прослушивании: ") + std::strerror(errno));
        }

        while (true)
        {
            memset(&client_struct, 0, sizeof(client_struct));
            socklen_t client_len = sizeof(client_struct);
            int client_fd = accept(server_fd, (sockaddr*)&client_struct, &client_len);
            if (client_fd == -1)
            {
                logger.write(Logging::Error, std::string("Ошибка при принятии соединения: ") + std::strerror(errno));
                continue;
            }

            Socket client_socket(client_fd);

            try
            {
                if (!authenticate(client_fd))
                {
                    continue;
                }
                
                std::uint32_t number_vectors;
                receiveExact(client_fd, &number_vectors, sizeof(number_vectors));

                if (number_vectors > MAX_VECTORS)
                {
                    throw std::out_of_range("Клиент передал большее количество векторов, чем требуется");
                }

                Calculator calc(number_vectors);
                for (std::uint32_t i = 0; i < number_vectors; ++i)
                {
                    auto vector = receiveVector(client_fd);
                    auto result = calc.calcVector(vector);

                    sendData(client_fd, &result, sizeof(result));
                }
            }
            catch (const std::out_of_range& ex)
            {
                try
                {
                    logger.write(Logging::Warn, ex.what());
                }
                catch (const std::exception& log_err)
                {
                    std::cerr << "Ошибка логгера: " << log_err.what() << '\n';
                }
            }
            catch (const std::exception& ex)
            {
                try
                {
                    logger.write(Logging::Error, ex.what());
                }
                catch (const std::exception& log_err)
                {
                    std::cerr << "Ошибка логгера: " << log_err.what() << '\n';
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        try
        {
            logger.write(Logging::Fatal, ex.what());
            logger.write(Logging::Info, "Остановка сервера...");
        }
        catch(const std::exception& ex)
        {
            std::cerr << ex.what() << '\n';
        }
        throw;
    }
}

void Server::receiveExact(int client_fd, void* data, ssize_t bytes)
{
    ssize_t recv_bytes = 0;
    while (recv_bytes < bytes)
    {
        ssize_t recv_byte = recv(client_fd, (char*)data + recv_bytes, bytes - recv_bytes, 0);
        if (recv_byte == -1)
        {
            throw std::runtime_error(std::string("Произошла ошибка при получении данных: ") + std::strerror(errno));
        }
        if (recv_byte == 0)
        {
            throw std::runtime_error("Были обработаны не все данные, клиент закрыл соединение!");
        }
        recv_bytes += recv_byte;
    }
}

void Server::sendData(int client_fd, const void* data, ssize_t to_send_bytes)
{
    ssize_t send_bytes = 0;
    while (send_bytes < to_send_bytes)
    {
        ssize_t bytes = send(client_fd, (const char*)data + send_bytes, to_send_bytes - send_bytes, MSG_NOSIGNAL);
        if (bytes <= 0)
        {
            throw std::runtime_error(std::string("Ошибка при отправке данных"));
        }
        send_bytes += bytes;
    }
}

bool Server::authenticate(int client_fd)
{
    char login_buff[4];
    receiveExact(client_fd, login_buff, sizeof(login_buff));

    std::string login(login_buff, sizeof(login_buff));

    User user;
    std::string salt = auth.genSalt();
    
    user = database.findUser(login);
    if (user.LOGIN.empty())
    {
        logger.write(Logging::Warn, std::string("Пользователь не найден в базе данных"));
        sendData(client_fd, "ERR", 3);
        return false;
    }
    sendData(client_fd, salt.data(), salt.size());
    
    char hash_buff[64];
    receiveExact(client_fd, hash_buff, sizeof(hash_buff));

    std::string hash(hash_buff, sizeof(hash_buff));

    if (auth.checkHash(salt, user.PASSWORD, hash))
    {
        sendData(client_fd, "OK", 2);
        return true;
    }
    logger.write(Logging::Warn, std::string("Пользователь ввёл неверный пароль"));
    sendData(client_fd, "ERR", 3);
    return false;
}

std::vector<std::uint16_t> Server::receiveVector(int client_fd)
{
    std::uint32_t size_vector;
    receiveExact(client_fd, &size_vector, sizeof(size_vector));

    if (size_vector > MAX_SIZE_VECTOR)
    {
        throw std::out_of_range("Клиент ввёл большое количество чисел");
    }

    std::vector<std::uint16_t> numbers;
    numbers.reserve(size_vector);

    for (std::uint32_t i = 0; i < size_vector; ++i)
    {
        std::uint16_t number;
        receiveExact(client_fd, &number, sizeof(number));
        numbers.push_back(number);
    }
    return numbers;
}