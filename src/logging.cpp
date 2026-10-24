#include "logging.h"

#include <stdexcept>

Logging::Logging(const std::string& path) : PATH(path)
{
    init();
}

void Logging::write(Logging::LevelLog level, const std::string& msg)
{
    time_t time = std::time(nullptr);
    char buf[80];
    auto tm = std::localtime(&time);
    if (!tm)
    {
        throw std::runtime_error("Преобразование времени не удалось!");
    }
    if (!strftime(buf, sizeof(buf), "%d.%m.%Y %T", tm))
    {
        throw std::logic_error("Недостаточно места для записи времени!");
    }

    std::ofstream file(PATH, std::ios::app);
    if (!file.is_open())
    {
        throw std::runtime_error("Не удалось открыть файл: " + PATH);
    }
    switch (level)
    {
    case Logging::Debug:
        file << "[DEBUG] " << buf << ' ' << msg << '\n';
        break;
    case Logging::Info:
        file << "[INFO]  " << buf << ' ' << msg << '\n';
        break;
    case Logging::Warn:
        file << "[WARN]  " << buf << ' ' << msg << '\n';
        break;
    case Logging::Error:
        file << "[ERROR] " << buf << ' ' << msg << '\n';
        break;
    case Logging::Fatal:
        file << "[FATAL] " << buf << ' ' << msg << '\n';
        break;
    }
    if (!file)
    {
        throw std::runtime_error("Ошибка записи в файл журнала: " + PATH);
    }
    file.close();
    if (!file)
    {
        throw std::runtime_error("Ошибка записи в файл журнала: " + PATH);
    }
}

void Logging::init()
{
    std::ofstream file(PATH, std::ios::app);
    if (!file.is_open())
    {
        throw std::runtime_error("Не удалось создать файл для логирования!");
    }
}
