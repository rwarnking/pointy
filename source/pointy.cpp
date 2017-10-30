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
		if (strcmp(argv[1], "-eval") == 0 )
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
		if (strcmp(argv[1], "-in") == 0 && strcmp(argv[3], "-out") == 0)
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