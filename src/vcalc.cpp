#include <UnitTest++/UnitTest++.h>

#include <cstdint>
#include <cctype>
#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <unistd.h>

#include "authenticator.h"
#include "calculator.h"
#include "cml.h"
#include "db.h"
#include "logging.h"

std::optional<std::string> read_file(const std::string& path)
{
    std::ifstream file(path);

    if (!file)
    {
        std::cerr << "Не удалось открыть файл" << '\n';
        return std::nullopt;
    }

    std::string file_content;
    std::string line;

    while(std::getline(file, line))
    {
        file_content += line + '\n';
    }

    file.close();
    return file_content;
}

SUITE(TEST_Calculator)
{
    TEST(NormalMultiplication)
    {
        Calculator calculator;
    
        std::vector<std::uint16_t> numbers = {1, 2, 3, 4, 5};
    
        CHECK_EQUAL(120, calculator.calcVector(numbers));
    }

    TEST(MultiplicationByOneElement)
    {
        Calculator calculator;
    
        std::vector<std::uint16_t> numbers = {10};
    
        CHECK_EQUAL(10, calculator.calcVector(numbers));
    }
    
    TEST(MultiplicationByZero)
    {
        Calculator calculator;
    
        std::vector<std::uint16_t> numbers = {2, 0, 5};
    
        CHECK_EQUAL(0, calculator.calcVector(numbers));
    }
    
    TEST(MultiplicationOverflow)
    {
        Calculator calculator;
    
        std::vector<std::uint16_t> numbers = {300, 300};
    
        CHECK_EQUAL(65535, calculator.calcVector(numbers));
    }
    
    TEST(MuliplicationMaximumValue)
    {
        Calculator calculator;
    
        std::vector<std::uint16_t> numbers = {65535, 1};
    
        CHECK_EQUAL(65535, calculator.calcVector(numbers));
    }
    
    TEST(MultiplicationByEmptyVector)
    {
        Calculator calculator;
    
        std::vector<std::uint16_t> numbers;
    
        CHECK_EQUAL(0, calculator.calcVector(numbers));
    }
    
    TEST(MultiplicationByOne)
    {
        Calculator calculator;
    
        std::vector<std::uint16_t> numbers = {1, 1, 1};
    
        CHECK_EQUAL(1, calculator.calcVector(numbers));
    }
}

SUITE(TEST_Authenticator)
{
    TEST(SaltLength)
    {
        Authenticator auth;
    
        std::string salt = auth.genSalt();
    
        CHECK_EQUAL(16, salt.size());
    }
    
    TEST(SaltIsHexString)
    {
        Authenticator auth;
    
        std::string salt = auth.genSalt();
    
        bool valid = true;
    
        for (char symbol : salt)
        {
            if (!std::isxdigit(static_cast<unsigned char>(symbol)))
            {
                valid = false;
                break;
            }
        }
    
        CHECK(valid);
    }
    
    TEST(SaltGeneration)
    {
        Authenticator auth;
    
        std::string salt1 = auth.genSalt();
        std::string salt2 = auth.genSalt();
    
        CHECK(salt1 != salt2);
    }
    
    TEST(HashLength)
    {
        Authenticator auth;
    
        std::string hash = auth.genHash("1234567890ABCDEF", "password");
    
        CHECK_EQUAL(64, hash.size());
    }
    
    TEST(VerifyHash)
    {
        Authenticator auth;
    
        std::string hash1 = auth.genHash("salt", "password");
        std::string hash2 = auth.genHash("salt", "password");
    
        CHECK_EQUAL(hash1, hash2);
    }
    
    TEST(DifferentPasswords)
    {
        Authenticator auth;
    
        std::string hash1 = auth.genHash("salt", "password1");
        std::string hash2 = auth.genHash("salt", "password2");
    
        CHECK(hash1 != hash2);
    }
    
    TEST(CorrectPassword)
    {
        Authenticator auth;
    
        std::string salt = "1234567890ABCDEF";
        std::string password = "password";
    
        std::string hash = auth.genHash(salt, password);
    
        CHECK(auth.checkHash(salt, password, hash));
    }
    
    TEST(IncorrectPassword)
    {
        Authenticator auth;
    
        std::string salt = "1234567890ABCDEF";
        std::string hash = auth.genHash(salt, "password");
    
        CHECK(!auth.checkHash(salt, "wrong_password", hash));
    }
    
    TEST(IncorrectHashLength)
    {
        Authenticator auth;
    
        CHECK(!auth.checkHash("salt", "password", "123"));
    }
}

SUITE(TEST_DB)
{

    TEST(LoadUsers)
    {
        const std::string path = "tests/test.conf"; 
    
        DB database(path);
        database.load();
    
        User user = database.findUser("ivan");
    
        CHECK_EQUAL(std::string("ivan"), user.LOGIN);
        CHECK_EQUAL(std::string("password1"), user.PASSWORD);
    
    }
    
    TEST(FindMissingUser)
    {
        const std::string path = "tests/test.conf"; 
    
        DB database(path);
        database.load();
    
        User user = database.findUser("unknown");
    
        CHECK(user.LOGIN.empty());
        CHECK(user.PASSWORD.empty());
    
    }
    
    TEST(EmptyFile)
    {
        const std::string path = "tests/empty.conf";
    
        DB database(path);
    
        database.load();
    
        User user = database.findUser("ivan");
    
        CHECK(user.LOGIN.empty());
    
    }
    
    TEST(MissingFile)
    {
        const std::string path = "";
    
        DB database(path);
    
        CHECK_THROW(database.load(), std::runtime_error);
    }

    TEST(MissingSeparator)
    {
        const std::string path = "tests/missing_separator.conf";

        DB database(path);

        CHECK_THROW(database.load(), std::invalid_argument);
    }
    
    TEST(EmptyLogin)
    {
        const std::string path = "tests/empty_login.conf";
    
        DB database(path);
    
        CHECK_THROW(database.load(), std::invalid_argument);
    }
    
    TEST(EmptyPassword)
    {
        const std::string path = "tests/empty_password.conf";
    
        DB database(path);
    
        CHECK_THROW(database.load(), std::invalid_argument);
    }
    
    TEST(DuplicateUser)
    {
        const std::string path = "tests/duplicate_user.conf";
    
        DB database(path);
    
        CHECK_THROW(database.load(), std::logic_error);
    }
}

SUITE(TEST_CML)
{

    TEST (DefaultArguments)
    {
        char arg0[] = "vcalc";
        char* argv[] = {arg0, nullptr};
    
        optind = 0;
        opterr = 0;

        cml args(1, argv);
    
        CHECK(args.parse());
    }
    
    TEST(SetPort)
    {
        char arg0[] = "vcalc";
        char arg1[] = "-p";
        char arg2[] = "12345";
    
        optind = 0;
        opterr = 0;

        char* argv[] = {arg0, arg1, arg2, nullptr};
    
        cml args(3, argv);
    
        CHECK(args.parse());
        CHECK_EQUAL(12345, args.getPort());
    }
    
    TEST(SetDatabasePath)
    {
        char arg0[] = "vcalc";
        char arg1[] = "-d";
        char arg2[] = "tests/test.conf";
    
        char* argv[] = {arg0, arg1, arg2, nullptr};
    
        optind = 0;
        opterr = 0;
        
        cml args(3, argv);
    
        CHECK(args.parse());
        CHECK_EQUAL(std::string("tests/test.conf"), args.getDbPath());
    }
    
    TEST(SetLogPath)
    {
        char arg0[] = "vcalc";
        char arg1[] = "-l";
        char arg2[] = "tests/test.log";
    
        char* argv[] = {arg0, arg1, arg2, nullptr};
    
        optind = 0;
        opterr = 0;
        
        cml args(3, argv);
    
        CHECK(args.parse());
        CHECK_EQUAL(std::string("tests/test.log"), args.getLogPath());
    }
    
    TEST(PortZero)
    {
        char arg0[] = "vcalc";
        char arg1[] = "-p";
        char arg2[] = "0";
    
        char* argv[] = {arg0, arg1, arg2, nullptr};
    
        optind = 0;
        opterr = 0;
        
        cml args(3, argv);
    
        CHECK(args.parse());
        CHECK_EQUAL(0, args.getPort());
    }
    
    TEST(NegativePort)
    {
        char arg0[] = "vcalc";
        char arg1[] = "-p";
        char arg2[] = "-1";
    
        char* argv[] = {arg0, arg1, arg2, nullptr};
    
        optind = 0;
        opterr = 0;
        
        cml args(3, argv);
    
        CHECK_THROW(args.parse(), std::invalid_argument);
    }
    
    TEST(PortTooLarge)
    {
        char arg0[] = "vcalc";
        char arg1[] = "-p";
        char arg2[] = "65536";
    
        char* argv[] = {arg0, arg1, arg2, nullptr};
    
        optind = 0;
        opterr = 0;
        
        cml args(3, argv);
    
        CHECK_THROW(args.parse(), std::invalid_argument);
    }
    
    TEST(NonNumericPort)
    {
        char arg0[] = "vcalc";
        char arg1[] = "-p";
        char arg2[] = "abc";
    
        char* argv[] = {arg0, arg1, arg2, nullptr};
    
        optind = 0;
        opterr = 0;
        
        cml args(3, argv);
    
        CHECK_THROW(args.parse(), std::invalid_argument);
    }
    
    TEST(PortWithCharacters)
    {
        char arg0[] = "vcalc";
        char arg1[] = "-p";
        char arg2[] = "123abc";
    
        char* argv[] = {arg0, arg1, arg2, nullptr};
    
        optind = 0;
        opterr = 0;
        
        cml args(3, argv);
    
        CHECK_THROW(args.parse(), std::invalid_argument);
    }
    
    TEST(UnknownOption)
    {
        char arg0[] = "vcalc";
        char arg1[] = "--unknown";
    
        char* argv[] = {arg0, arg1};
    
        optind = 0;
        opterr = 0;

        cml args(2, argv);
    
        CHECK_THROW(args.parse(), std::invalid_argument);
    }
}

SUITE(TEST_LOGGING)
{

    TEST(Initialization)
    {
        std::string path = "tests/test.log";
    
        {
            Logging logger(path);
    
            auto content = read_file(path);
    
            if (!content)
            {
                CHECK(false);
                return;
            }
    
            CHECK((content.value()).find("[INFO]") != std::string::npos);
            CHECK((content.value()).find("Запуск сервера...") != std::string::npos);
        }
    }
    
    TEST(DebugMessage)
    {
        std::string path = "tests/test.log";
    
        {
            Logging logger(path);
            logger.write(Logging::Debug, "Debug message");
    
            auto content = read_file(path);
    
            if (!content)
            {
                CHECK(false);
                return;
            }
    
            CHECK((content.value()).find("[DEBUG] Debug message") != std::string::npos);
        }
    }
    
    TEST(InfoMessage)
    {
        std::string path = "tests/test.log";
    
        {
            Logging logger(path);
            logger.write(Logging::Info, "Info message");
    
            auto content = read_file(path);
    
            if (!content)
            {
                CHECK(false);
                return;
            }
    
            CHECK((content.value()).find("[INFO] Info message") != std::string::npos);
        }
    }
    
    TEST(WarningMessage)
    {
        std::string path = "tests/test.log";
    
        {
            Logging logger(path);
            logger.write(Logging::Warn, "Warning message");
    
            auto content = read_file(path);
    
            if (!content)
            {
                CHECK(false);
                return;
            }
    
            CHECK((content.value()).find("[WARN] Warning message") != std::string::npos);
        }
    }
    
    TEST(ErrorMessage)
    {
        std::string path = "tests/test.log";
    
        {
            Logging logger(path);
            logger.write(Logging::Error, "Error message");
    
            auto content = read_file(path);
    
            if (!content)
            {
                CHECK(false);
                return;
            }
    
            CHECK((content.value()).find("[ERROR] Error message") != std::string::npos);
        }
    }
    
    TEST(FatalMessage)
    {
        std::string path = "tests/test.log";
    
        {
            Logging logger(path);
            logger.write(Logging::Fatal, "Fatal message");
    
            auto content = read_file(path);
    
            if (!content)
            {
                CHECK(false);
                return;
            }
    
            CHECK((content.value()).find("[FATAL] Fatal message") != std::string::npos);
        }
    }
}

int main()
{
    return UnitTest::RunAllTests();
}