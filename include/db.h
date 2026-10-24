#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

struct User
{
    std::string LOGIN;
    std::string PASSWORD;
};

class db
{
private:
const std::string PATH;
std::unordered_map<std::string, std::string> users;
public:
    db(const std::string& path);
    void load();
    User findUser(const std::string& login);
};