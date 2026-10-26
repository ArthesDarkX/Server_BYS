#include "logging.h"

#include <stdexcept>

Logging::Logging(const std::string& path) : PATH(path)
{
    init();
}

void Logging::init()
{
    Logging::write(LevelLog::Info, "Запуск сервера...");
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
    if (!strftime(buf, sizeof(buf), "%d-%m-%Y %T", tm))
    {
        throw std::out_of_range("Недостаточно места для записи времени!");
    }

    std::ofstream file(PATH, std::ios::app);
    if (!file.is_open())
    {
        throw std::runtime_error("Не удалось открыть файл: " + PATH);
    }
    switch (level)
    {
    case Logging::Debug:
        file << buf << " [DEBUG] " << msg << '\n';
        break;
    case Logging::Info:
        file << buf << " [INFO] " << msg << '\n';
        break;
    case Logging::Warn:
        file << buf << " [WARN] " << msg << '\n';
        break;
    case Logging::Error:
        file << buf << " [ERROR] " << msg << '\n';
        break;
    case Logging::Fatal:
        file << buf << " [FATAL] "  << msg << '\n';
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