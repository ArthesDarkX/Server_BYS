#pragma once

#include <string>

class Authenticator
{
private:
public:
    std::string genSalt();
    std::string genHash(const std::string& salt, const std::string& password);
    bool checkHash(const std::string& salt, const std::string& password, const std::string& hash_client);
};