#pragma once

#include <getopt.h>
#include <string>

struct option long_options[] = {
    {"help", no_argument, nullptr, 'h'},
    {"database", required_argument, nullptr, 'd'},
    {"port", required_argument, nullptr, 'p'},
    {"log", required_argument, nullptr, 'l'},
    {0, 0, 0, 0}
};

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
};