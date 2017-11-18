#include "../header/problem.h"

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
			bool print = string(argv[5]).compare("-p") == 0 || (argc == 7 && string(argv[6]).compare("-p") == 0);
			short algo = 0;
			if (string(argv[5]).compare("-p") != 0)
				algo = string(argv[5]).compare("-d") == 0 ? 1 : (string(argv[5]).compare("-t") == 0 ? 2 : 0);
			else if (argc == 7 && string(argv[6]).compare("-p") != 0)
				algo = string(argv[6]).compare("-d") == 0 ? 1 : (string(argv[6]).compare("-t") == 0 ? 2 : 0);

			prob.GenerateSolution(argv[2], argv[4], print, 1);
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