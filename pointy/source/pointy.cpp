#include "../header/problem.h"
#include "../header/logger.h"

using namespace std;
using namespace logger;

// Gebrauchshilfe
void PrintHelp(void)
{
	// Logger::Println(LEVEL::INFO, "This is the help function.");
	// Logger::Println(LEVEL::INFO, "This program enables you to evaluate or generate a solution for the Labeling Problem.");
	// Logger::Println(LEVEL::INFO, "The evaluation process can be started by writing: 'pointy -eval filename.txt' ");
	// Logger::Println(LEVEL::INFO, "The generation process can be started by writing: 'pointy -in inputfilename.txt -out outputfilename.txt' ");
	// Logger::Println(LEVEL::INFO, "The generation process can be started and exported to a bmp by writing: 'pointy -in inputfilename.txt -out outputfilename.txt -p' ");
	// Logger::Println(LEVEL::INFO, "The related files will be generated in the data-folder. Therefore the inputfiles should be located in the same folder.");

	Logger::Println(LEVEL::INFO, "Description: This program can solve a 4-point-labelling problem or evaluate a given solution for such a problem\n");
	Logger::Println(LEVEL::INFO, "Evaluate   : pointy -eval <filename>");
	Logger::Println(LEVEL::INFO, "Solve      : pointy -in <infile> -out <outfile> [-p] [-g|-s|-i]");
	Logger::Println(LEVEL::INFO, "------------------------------------------------------------------------");
	Logger::Println(LEVEL::INFO, "Flags:");
	Logger::Println(LEVEL::INFO, "   -p      | generate a .bmp image for the computed solution");
	Logger::Println(LEVEL::INFO, "   -i      | use idiot heuristic (default)");
	Logger::Println(LEVEL::INFO, "   -g      | use graphic heuristic");
	Logger::Println(LEVEL::INFO, "   -s      | use simulated annealing");
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
	//Logger::SetLogLevel(INFO);

	Problem prob = Problem();

	if (argc == 3)
	{
		if (string(argv[1]).compare("-eval") == 0 )
		{
			prob.CheckSolution(argv[2]);
		}
		else
		{
			Logger::Println(LEVEL::ERR ,"Invalid input parameters using '-eval'");
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
			Logger::Println(LEVEL::ERR, "Invalid input parameters using '-in' and '-out'");
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
			Logger::Println(LEVEL::ERR, "Invalid input parameters using '-in' and '-out'");
		}
	}
	else
	{
		PrintHelp();
	}

	return 0;
}