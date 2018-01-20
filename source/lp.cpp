#include "../header/solver.h"
#include "../header/lp_tester.h"

#include <cstring>

int main(int argc, char **argv)
{
	if (argc >= 2)
	{
		Solver s;
		if (strcmp(argv[1], "-test") == 0)
		{
			int iter = 1;
			// .exe -test indir outfile
			if (argc == 4)
			{
				TestDir(argv[2], argv[3], iter);
			}
			// .exe -test <-p> indir outfile <iter>
			else if (argc <= 6)
			{
				bool draw = strcmp(argv[2], "-p") == 0;
				int iter_index = draw ? 5 : 4;
				if (argc == 6)
				{
					try
					{
						iter = std::stoi(argv[iter_index]);
					}
					catch (...)
					{
						// stuff
					}
				}
				TestDir(argv[iter_index - 2], argv[iter_index - 1], iter, draw);
			}
		}
		else
		{
			bool draw = strcmp(argv[1], "-p") == 0;
			Solver s = Solver(draw ? argv[2] : argv[1]);
			s.Solve(argc == 3 ? argv[2] : "out.txt", false, false, draw);
		}
	}

	return 0;
}