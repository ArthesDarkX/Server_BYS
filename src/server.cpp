#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <cerrno>

#include "server.h"
#include "calculator.h"

void server::start()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        throw std::runtime_error("Произошла ошибка при создании сокета!\n");
    }

    memset(&server_struct, 0, sizeof(server_struct));
    server_struct.sin_addr.s_addr = INADDR_ANY;
    server_struct.sin_family = AF_INET;
    server_struct.sin_port = htons(port);
    
    if ((bind(server_fd, (sockaddr*)&server_struct, sizeof(server_struct))) == -1)
    {
        close(server_fd);
        throw std::runtime_error("Произошла ошибка bind!\n");
    }

    if ((listen(server_fd, 1)) == -1)
    {
        close(server_fd);
        throw std::runtime_error("Произошла ошибка listen!\n");
    }

    while (true)
    {
        memset(&client_struct, 0, sizeof(client_struct));
        int client_fd = accept(server_fd, (sockaddr*)&client_struct, &client_len);
        if (client_fd == -1)
        {
            switch (errno)
            {
            case EINTR:
                continue;
            default:
                throw std::runtime_error("Произошла ошибка при принятии соединения!");
                break;
            }
        }
        try
        {
            receiveData(client_fd);

            calculator calc(data.size());
            auto result = calc.calc(data);
            auto count_result = htonl(result.first);
            sendData(client_fd, &count_result, sizeof(count_result));
            for (auto el : result.second)
            {
                auto element = htons(el);
                sendData(client_fd, &element, sizeof(el));
            }
        }
        catch (const std::runtime_error& ex)
        {
            std::cerr << ex.what() << '\n';
        }
        catch (const std::out_of_range& ex)
        {
            std::cerr << ex.what() << '\n';
        }
        close(client_fd);
    }
}

void server::receiveExact(int client_fd, void* data, ssize_t bytes)
{
    ssize_t recv_bytes = 0;
    while (recv_bytes < bytes)
    {
        ssize_t recv_byte = recv(client_fd, reinterpret_cast<char*>(data) + recv_bytes, bytes - recv_bytes, 0);
        if (recv_byte == -1)
        {
            throw std::runtime_error("Произошла ошибка при получении данных\n");
        }
        if (recv_byte == 0)
        {
            throw std::runtime_error("Были обработаны не все данные, клиент закрыл соединение!");
        }
        recv_bytes += recv_byte;
    }
}

void server::receiveData(int client_fd)
{
    data.clear();
    std::uint32_t number_vectors;
    receiveExact(client_fd, &number_vectors, sizeof(number_vectors));
    number_vectors = ntohl(number_vectors);
    if (number_vectors > MAX_VECTORS)
    {
        throw std::out_of_range("Введено большое количество векторов!");
    }

    std::uint32_t vectors{};
    while (vectors < number_vectors)
    {
        std::uint32_t size_vector;
        receiveExact(client_fd, &size_vector, sizeof(size_vector));
        size_vector = ntohl(size_vector);
        if (size_vector > MAX_SIZE_VECTOR)
        {
            throw std::out_of_range("Введено большое количество чисел в векторе!");
        }

        std::vector<std::uint16_t> v{};
        while (v.size() != size_vector)
        {
            std::uint16_t number;
            receiveExact(client_fd, &number, sizeof(number));
            number = ntohs(number);
            v.push_back(number);
        }
        data.push_back(v);
        ++vectors;
    }
}

void server::sendData(int client_fd, void* data, ssize_t to_send_bytes)
{
    ssize_t send_bytes = 0;
    while (send_bytes < to_send_bytes)
    {
        ssize_t bytes = send(client_fd, reinterpret_cast<char*>(data) + send_bytes, to_send_bytes - send_bytes, 0);
        if (bytes <= 0)
        {
            throw std::runtime_error("Не удалось отправить результат");
        }
        send_bytes += bytes;
    }
}