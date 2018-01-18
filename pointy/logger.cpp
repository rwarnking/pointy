#include "../header/logger.h"

namespace logger
{

    LEVEL Logger::LogLevel()
    {
        return loglevel;
    }

    void Logger::SetLogLevel(LEVEL lvl)
    {
        loglevel = lvl;
    }

    template <>
    void Logger::Print(LEVEL lvl, std::string message)
    {
        if (lvl <= loglevel)
        {
            cout << message;

            if (lvl == ERR)
                exit(-1);
        }
    }

    template <>
    void Logger::Print(LEVEL lvl, std::string message, std::string... args)
    {
        if (lvl <= loglevel)
        {
            cout << message;

            Print(lvl, args...);
        }
    }

    template <>
    void Logger::Println(LEVEL lvl, std::string message)
    {
        if (lvl <= loglevel)
        {
            cout << message << endl;

            if (lvl == ERR)
                exit(-1);
        }
    }

    template <>
    void Logger::Println(LEVEL lvl, std::string message, std::string... args)
    {
        if (lvl <= loglevel)
        {
            cout << message;

            Println(lvl, args...);
        }
    }

    void Logger::Println(LEVEL lvl)
    {
        if (lvl <= loglevel)
        {
            cout  << endl;

            if (lvl == ERR)
                exit(-1);
        }
    }
}