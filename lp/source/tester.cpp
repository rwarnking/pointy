#include "../header/tester.h"
#include "../header/logger.h"
#include "../header/solver.h"

#include <string>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

using namespace std;

void TestFile(const char *infile, const char *outfile)
{
    double *time;
    Solver solver = Solver(infile);
    int obj_value = solver.Solve("", time, true, false, false);

    try
    {
        ofstream of(infile, ios::app);
        of << "\n" << infile << "\n" << obj_value << " " << time << "\n";
        of.close();
    }
    catch (exception)
    {
        logger::Logger::Println(logger::LEVEL::ERR, "ERROR: could not write to file: ", infile);
    }
}

void TestDir(const char *indir, const char *outfile)
{
    logger::Logger::Println(logger::LEVEL::INFO, "Testing all files in <", indir, "> ...");

    // Get all files in current dir and every subdir (recursive)
#ifdef _WIN32
    HANDLE dir;
    WIN32_FIND_DATA file_data;
    string directory(indir);

    if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return; /* No files found */

    do {
        const string file_name = file_data.cFileName;
        const string full_file_name = directory + "/" + file_name;
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (file_name[0] == '.')
            continue;

        if (is_directory)
            continue;

        logger::Logger::Println(logger::LEVEL::INFO, "Testing file <", full_file_name, "> ...");

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

        logger::Logger::Println(logger::LEVEL::INFO, "Testing file <", full_file_name, "> ...");

        TestFile(full_file_name.c_str(), outfile);
    }
    closedir(dir);
#endif
}