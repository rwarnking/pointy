#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <iostream>

using namespace std;

namespace logger
{
    enum LEVEL : short
    {
        INFO = 0,
        ERR,
        DEBUG
    };

    class Logger
    {
    public:

        static LEVEL LogLevel()
        {
            return loglevel;
        }

        static void Println(LEVEL lvl)
        {
            if (lvl <= loglevel)
            {
                cout  << endl;

                if (lvl == ERR)
                    exit(-1);
            }
        }

        template <typename T>
        static void Print(LEVEL lvl, T message)
        {
            if (lvl <= loglevel)
            {
                cout << message;

                if (lvl == ERR)
                    exit(-1);
            }
        }

        template <typename T, typename... Args>
        static void Print(LEVEL lvl, T message, Args... args)
        {
            if (lvl <= loglevel)
            {
                cout << message;

                Print(lvl, args...);
            }
        }

        template <typename T>
        static void Println(LEVEL lvl, T message)
        {
            if (lvl <= loglevel)
            {
                cout << message << endl;

                if (lvl == ERR)
                    exit(-1);
            }
        }

        template <typename T, typename... Args>
        static void Println(LEVEL lvl, T message, Args... args)
        {
            if (lvl <= loglevel)
            {
                cout << message;

                Println(lvl, args...);
            }
        }


    private:
        
        static const LEVEL loglevel = INFO;

    };
}

#endif /* _LOGGER_H_  */
