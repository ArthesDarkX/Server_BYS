#include <iostream>
#include <exception>

#include "authenticator.h"
#include "cml.h"
#include "db.h"
#include "logging.h"
#include "server.h"

int main(int argc, char* argv[])
{
    try
    {
        cml args(argc, argv);

        if (!args.parse())
        {
            return 0;
        }

        Logging logger(args.getLogPath());

        DB database(args.getDbPath());
        database.load();

        Authenticator auth;

        Server server(args.getPort(), database, auth, logger);
        server.start();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    return 0;
}
