#pragma once

#include <ctime>
#include <fstream>
#include <string>

class Logging
{
private:
const std::string PATH;
public:
enum LevelLog
{
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};
    Logging()=delete;
    Logging(const std::string& path);
    void write(Logging::LevelLog level, const std::string& msg);
    void init();
};