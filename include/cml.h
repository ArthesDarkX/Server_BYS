#pragma once

#include <getopt.h>
#include <string>

extern struct option long_options[];

class cml
{
private:
std::string PATH_DB = "/etc/vcalc.conf";
std::string PATH_LOG = "/var/log/vcalc.log";
unsigned PORT = 33333;

const int arg_count;
char** arg_values;
public:
    cml(int argc, char** argv);
    bool parse();
    void help();
    const std::string& getDbPath() const { return PATH_DB; }
    const std::string& getLogPath() const { return PATH_LOG; }
    unsigned getPort() const { return PORT; }
};