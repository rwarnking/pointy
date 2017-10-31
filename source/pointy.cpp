#include "../header/problem.h"

using namespace std;

// Gebrauchshilfe
void PrintHelp(void)
{
	cout << "You are stupid" << endl;
}

int main(int argc, char **argv)
{
	Problem prob = Problem();

	if (argc == 3)
	{
		if (string(argv[1]).compare("-eval") == 0 )
		{
			prob.CheckSolution(argv[2]);
		}
		else
		{
			cout << "Invalid input parameters using '-eval'" << endl;
			PrintHelp();
		}
	}
	else if (argc == 5)
	{
		if (string(argv[1]).compare("-in") == 0 && string(argv[3]).compare("-out") == 0)
		{
			prob.GenerateSolution(argv[2], argv[4]);
		}
		else
		{
			cout << "Invalid input parameters using '-in' and '-out'" << endl;
			PrintHelp();
		}
	}
	else
	{
		PrintHelp();
	}

	return 0;
}