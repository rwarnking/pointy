#include "../header/tester.h"
#include "../header/logger.h"
#include "../header/solver.h"

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

using namespace std;

void TestFile(const char *infile, const char *outfile, int iterations)
{
    vector<double> times = vector<double>();
    int obj_value;
    Solver solver = Solver(infile);

    double mean = 0.0;
    try
    {
        ofstream of(outfile, ios::app);
        for (int i = 0; i < iterations; i++)
        {
            times.push_back(0.0);
            obj_value = solver.Solve("", &times.back(), true, false, false);
            mean += times.back();
        }
        sort(times.begin(), times.end(), [](double a, double b) { return a < b; });

        of << "\n" << obj_value << " " << mean / (double)iterations << " " << times[times.size()/2] << " (" << infile << ")\n";
        of.close();
    }
    catch (...)
    {
        logger::Logger::Println(logger::LEVEL::ERR, "ERROR: could not write to file: ", outfile);
    }
}

void TestDir(const char *indir, const char *outfile, int iterations)
{
    logger::Logger::Println(logger::LEVEL::INFO, "Testing all files in < ", indir, " > ...");

    // Get all files in current dir and every subdir (recursive)
#ifdef _WIN32
    HANDLE dir;
    WIN32_FIND_DATA file_data;
    string directory(indir);

    if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return; /* No files found */

    try
    {
        ofstream of(outfile, ios::app);
        of << "ITERATIONS = " << iterations << "\n<objective value> " << "<mean time> " << "<median time> " << "(<filename>)\n";
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

        logger::Logger::Println(logger::LEVEL::INFO, "Testing file < ", file_name, " > ...");

        TestFile(full_file_name.c_str(), outfile);;
    } while (FindNextFile(dir, &file_data));

    FindClose(dir);
#else
    DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(directory);
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

        logger::Logger::Println(logger::LEVEL::INFO, "Testing file < ", file_name, " > ...");

        TestFile(full_file_name.c_str(), outfile);
    }
    closedir(dir);
#endif
}