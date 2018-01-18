#include "../header/solver.h"
#include "../header/tester.h"

#include <cstring>

int main(int argc, char **argv)
{  
    if (argc == 2 || argc == 3)
    {
        Solver s = Solver(argv[1]);
        s.Solve(argc == 3 ? argv[2] : "out.txt");
    }
    else if (argc == 4)
    {
        if (strcmp(argv[1], "-test") == 0)
            TestDir(argv[2], argv[3]);
    }

    return 0;
}