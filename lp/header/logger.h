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

        static const LEVEL loglevel = DEBUG;

        static LEVEL LogLevel()
        {
            return loglevel;
        }

        static void Println(LEVEL lvl)
        {
            if (lvl <= loglevel)
                cout  << endl;
        }

        template <typename T>
        static void Print(LEVEL lvl, T message)
        {
            if (lvl <= loglevel)
                cout << message;
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
                cout << message << endl;
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

        template <typename T>
        static void PrintAbort(LEVEL lvl, T message)
        {
            if (lvl <= loglevel)
            {
                cout << message;

                if (lvl == ERR)
                    exit(-1);
            }
        }

        template <typename T, typename... Args>
        static void PrintAbort(LEVEL lvl, T message, Args... args)
        {
            if (lvl <= loglevel)
            {
                cout << message;

                PrintAbort(lvl, args...);
            }
        }

        template <typename T>
        static void PrintlnAbort(LEVEL lvl, T message)
        {
            if (lvl <= loglevel)
            {
                cout << message << endl;

                if (lvl == ERR)
                    exit(-1);
            }
        }

        template <typename T, typename... Args>
        static void PrintlnAbort(LEVEL lvl, T message, Args... args)
        {
            if (lvl <= loglevel)
            {
                cout << message;

                PrintlnAbort(lvl, args...);
            }
        }
    };
}

#endif /* _LOGGER_H_  */
