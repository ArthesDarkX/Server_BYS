#include <cstring>
#include <iostream>
#include <stdexcept>

#include "cml.h"

cml::cml(int argc, char** argv) : arg_count(argc), arg_values(argv) {};

bool cml::parse()
{
    int opt;
    while ((opt = getopt_long(arg_count, arg_values, "hd:p:l:", long_options, nullptr)) != -1)
    {
        switch (opt)
        {
            case 'h':
                help();
                return false;
            case 'd':
                PATH_DB = optarg;
                break;
            case 'p':
                {
                    size_t pos;
                    int port;
                    try
                    {
                        port = std::stoi(optarg, &pos);
                    }
                    catch (const std::invalid_argument)
                    {
                        throw std::invalid_argument("Указан некорректный порт: " + std::string(optarg));
                    }
                    catch (const std::out_of_range)
                    {
                        throw std::out_of_range("Порт больше допустимого значения. Порт: " + std::string(optarg));
                    }
                    if (pos != std::strlen(optarg))
                    {
                       throw std::invalid_argument("Указан некорректный порт: " + std::string(optarg));
                    }
                    if (port < 0)
                    {
                        throw std::invalid_argument("Порт меньше 0");
                    }
                    if (port > 65535)
                    {
                        throw std::invalid_argument("Порт больше допустимого значения!");
                    }
                    PORT = port;
                    break;
                }
            case 'l':
                PATH_LOG = optarg;
                break;
            case '?':
                throw std::invalid_argument("Указана неизвестная опция!");
        }
    }
    return true;
}

void cml::help()
{
    std::cout << "Справка использования программы\n"
    << "./vcalc - запуск по умолчанию"
    << "-h (--help) - вывод справки\n"
    << "-p (--port) [port] - установка порта для запуска сервера, по умолчанию 33333\n"
    << "-d (--database) [path] - указание пути для файла базы данных, по умолчанию vcalc.conf\n"
    << "-l (--log) [path] - указание путь для файла логирования, по умолчанию vcalc.log\n";
}
