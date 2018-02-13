#include "../header/solver.h"
#include "../header/lp_tester.h"
#include "../header/logger.h"

#include <cstring>

int main(int argc, char **argv)
{
	if (argc >= 4)
	{
		if (strcmp(argv[1], "-test") == 0)
		{
			int iter = 1;
			// .exe -test indir outfile
			if (argc == 4)
			{
				TestDir(argv[2], argv[3], iter);
			}
			// .exe -test <-p> <-c> indir outfile <iter>
			else if (argc <= 7)
			{
				bool draw = strcmp(argv[2], "-p") == 0;
				bool cliques = strcmp(argv[(int)draw + 2], "-c") == 0;
				int iter_index = (int)draw + (int)cliques + 4;
				if (argc >= 6)
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
				TestDir(argv[iter_index - 2], argv[iter_index - 1], iter, draw, cliques);
			}
		}
		// .exe <-p> <-c> infile outfile
		else
		{
			bool draw = strcmp(argv[1], "-p") == 0;
			bool cliques = strcmp(argv[(int)draw + 1], "-c") == 0;
			int index = (int)draw + (int)cliques + 1;
			Solver s = Solver(argv[index], cliques);
			s.Solve(argv[index+1], false, true, draw);
		}
	}
	// .exe -ctest infile
	else if (argc == 3)
	{
		if (strcmp(argv[1], "-ctest") == 0)
		{
			TestCliques(argv[2]);
		}
		else
		{
			Solver s = Solver(argv[1]);
			s.Solve(argv[2]);
		}
	}
	else
	{
		// TODO: usage
		logger::Logger::Println(logger::LEVEL::ERR, "Invalid commands");
	}

	return 0;
}