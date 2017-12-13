#include "../header/problem.h"

#include <iostream>

using namespace std;

// Gebrauchshilfe
void PrintHelp(void)
{
	cout << "This is the help function." << endl;
	cout << "This program enables you to evaluate or generate a solution for the Labeling Problem." << endl;
	cout << "The evaluation process can be started by writing: 'pointy -eval filename.txt' " << endl;
	cout << "The generation process can be started by writing: 'pointy -in inputfilename.txt -out outputfilename.txt' " << endl;
	cout << "The generation process can be started and exported to a bmp by writing: 'pointy -in inputfilename.txt -out outputfilename.txt -p' " << endl;
	cout << "The related files will be generated in the data-folder. Therefore the inputfiles should be located in the same folder." << endl;
}

void ParseFlags(char **argv, int argc, bool &print, ALGORITHM &algorithm)
{
	bool found = false;
	for (int i = 5; i < argc; i++)
	{
		if (string(argv[i]).compare("-p") == 0)
			print = true;
		else if (!found && string(argv[i]).compare("-g") == 0)
			algorithm = GRAPHIC;
		else if (!found && string(argv[i]).compare("-s") == 0)
			algorithm = SIMULATED_ANNEALING;
		else if (!found && string(argv[i]).compare("-i") == 0)
			algorithm = IDIOT;
	}
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
	else if (argc >= 6)
	{
		if (string(argv[1]).compare("-in") == 0 && string(argv[3]).compare("-out") == 0)
		{
			bool print = false;
			ALGORITHM algorithm = IDIOT;
			ParseFlags(argv, argc, print, algorithm);
			
			prob.GenerateSolution(argv[2], argv[4], print, algorithm);
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