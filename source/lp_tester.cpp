#include "../header/lp_tester.h"
#include "../header/logger.h"
#include "../header/solver.h"

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

using namespace std;

bool TestFile(const char *infile, const char *outfile, int iterations, bool draw)
{
	int obj_value;
    vector<chrono::duration<double>> times = vector<chrono::duration<double>>();

    // Construct instance and graph, measue time it took
    auto cstart = chrono::high_resolution_clock::now();
	Solver solver = Solver(infile);
    auto construct_time = chrono::duration<double>(chrono::high_resolution_clock::now() - cstart);

    bool correct = true;    
	chrono::duration<double> mean = chrono::high_resolution_clock::now() - chrono::high_resolution_clock::now();
    // Measure solving time for <iterations> iterations
	for (int i = 1; i <= iterations; i++)
	{
        auto start = chrono::high_resolution_clock::now();
		obj_value = solver.Solve("", false, false, draw, i);
        auto exec_time = chrono::duration<double>(chrono::high_resolution_clock::now() - start);
        times.push_back(exec_time+construct_time);

		// Test whether result is correct (not testing whether it's optimal)
        correct = correct && solver.CheckInstance();
		mean += times.back();
	}
    // Sort measured times in order to get the median
	sort(times.begin(), times.end(), [](chrono::duration<double> a, chrono::duration<double> b) { return a < b; });

    try
    {
        // Write results to <outfile>
        ofstream of(outfile, ios::app);
		of << "\n" << setw(10) << obj_value << " " << setw(10) << construct_time.count() << " "
           << setw(10) << (mean / (double)iterations).count() << " " << setw(10)
           << times[times.size() / 2].count() << " (" << infile << ")\n";
		of.close();
	}
	catch (...)
	{
		logger::Logger::Println(logger::LEVEL::ERR, "ERROR: could not write to file: ", outfile);
		return false;
	}

	return correct;
}

void TestDir(const char *indir, const char *outfile, int iterations, bool draw)
{
	logger::Logger::Println(logger::LEVEL::INFO, "Testing all files in < ", indir, " > ...");

	// Get all files in current dir
#ifdef _WIN32
	// Windows
	HANDLE dir;
    WIN32_FIND_DATA file_data;
    string directory(indir);

    if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return;

    try
    {
        ofstream of(outfile, ios::app);
        of << setw(5) << "// iterations = " << iterations << " -- [objective value]"
           << " [preparation time]" << " [mean time]" << " [median time]" << " (filename)\n";
        of.close();
    }
    catch (...)
    {
        logger::Logger::Println(logger::LEVEL::ERR, "ERROR: could not write to file: ", outfile);
    }

    do {
        const string file_name = file_data.cFileName;
        const string full_file_name = directory + "/" + file_name;

        if (file_name[0] == '.' || (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            continue;

        logger::Logger::Print(logger::LEVEL::INFO, "Testing file < ", file_name, " > ...");
        bool test = TestFile(full_file_name.c_str(), outfile, iterations, draw);
        logger::Logger::Println(logger::LEVEL::INFO, (test ? " OKAY" : " INCORRECT"));

    } while (FindNextFile(dir, &file_data));

    FindClose(dir);
#else
	// Unix 
	DIR *dir;
	class dirent *ent;
	class stat st;

	dir = opendir(directory);

	try
    {
        ofstream of(outfile, ios::app);
        of << "// iterations = " << iterations << " -- [objective value]"
           << " [preparation time]" << " [mean time]" << " [median time]" << " (filename)\n";
        of.close();
    }
    catch (...)
    {
        logger::Logger::Println(logger::LEVEL::ERR, "ERROR: could not write to file: ", outfile);
    }

	while ((ent = readdir(dir)) != NULL) {
		const string file_name = ent->d_name;
		const string full_file_name = directory + "/" + file_name;

		if (file_name[0] == '.')
			continue;

		if (stat(full_file_name.c_str(), &st) == -1)
			continue;

		const bool is_directory = (st.st_mode & S_IFDIR) != 0;

		if (is_directory)
			continue;

		logger::Logger::Print(logger::LEVEL::INFO, "Testing file < ", file_name, " > ...");
        bool test = TestFile(full_file_name.c_str(), outfile, iterations, draw);
        logger::Logger::Println(logger::LEVEL::INFO, (test ? " OKAY" : " INCORRECT"));
	}
	closedir(dir);
#endif
}