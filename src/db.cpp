#include <stdexcept>

#include "db.h"

db::db(const std::string& path) : PATH(path) {};

void db::load()
{
    std::ifstream file(PATH);
    if (!file.is_open())
    {
        throw std::runtime_error("Не удалось открыть файл базы данных!");
    }
    std::string login;
    std::string password;
    std::string temp;
    while (std::getline(file, temp))
    {
        std::size_t index = temp.find(':');
        if (index != std::string::npos)
        {
            login = temp.substr(0, index);
            password = temp.substr(index+1);
        }
        else
        {
            throw std::invalid_argument("В строке нет разделителя \":\"");
        }
        if (login.empty())
        {
            throw std::invalid_argument("Логин пуст!");
        }
        if (password.empty())
        {
            throw std::invalid_argument("Пароль пуст!");
        }
        if (users.find(login) != users.end()) {
            throw std::logic_error("Пользователь уже есть в базе!");
        }
        else
        {
            users.emplace(login, password);
        }
    }
}

User db::findUser(const std::string& login)
{
    User user;
    auto it = users.find(login);
    if (it != users.end())
    {
        user.LOGIN = it->first;
        user.PASSWORD = it->second;
    }
    else
    {
        throw std::logic_error("Пользователь " + login + " не найден!");
    }
    return user;
}