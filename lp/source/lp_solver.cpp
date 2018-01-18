#include "../header/solver.h"
#include "../header/tester.h"

#include <cstring>

int main(int argc, char **argv)
{  
    if (argc == 2 || argc == 3)
    {
        Solver s = Solver(argv[1]);
        //s.Solve(argc == 3 ? argv[2] : "out.txt", nullptr, false, true, true);
        s.Solve(argc == 3 ? argv[2] : "out.txt", nullptr, false, false, false);
    }
    else if (argc >= 4)
    {
        int iter = 1;
        if (strcmp(argv[1], "-test") == 0)
        {
            if (argc == 5)
            {
                try
                {
                    iter = std::stoi(argv[4]);
                }
                catch (...)
                {
                    // stuff
                }
            }
            TestDir(argv[2], argv[3], iter);
        }
    }

    return 0;
}