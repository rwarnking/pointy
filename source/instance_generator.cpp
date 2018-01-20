#include "../header/instance_generator.h"
#include "../header/logger.h"

using namespace std;
using namespace logger;

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        try
        {
            int points = stoi(argv[2]);
            GenerateInstance(argv[1], points);
        }
        catch(...)
        {
            Logger::PrintlnAbort(LEVEL::ERR, "ERROR: invalid point count argument");
        }
    }
    else if (argc >= 5)
    {
        try
        {
            int points = stoi(argv[2]);
            int min = stoi(argv[3]);
            int max = stoi(argv[4]);

            if (min > max)
            {
                Logger::Println(LEVEL::ERR, "ERROR: min value must be less or equal to max value: min = ", min, ", max = ", max);
                PrintHelp();
                return -1;
            }

            bool even = true;
            if (argc == 6) 
                even = stoi(argv[5]) == 1;

            GenerateInstance(argv[1], points, min, max, even);
        }
        catch(...)
        {
            Logger::PrintlnAbort(LEVEL::ERR, "ERROR: invalid point count argument");
        }
    }
    else
    {
        PrintHelp();
    }

    return 0;
}

void PrintHelp()
{
    Logger::Println(LEVEL::INFO, "This program generates a problem instance for the 4-corner labelling problem\n");
    Logger::Println(LEVEL::INFO, "Usage: instance_generator <filename> <number of points> [min value] [max value] [uniform distribution{0=false,1=true}]");
    Logger::Println(LEVEL::INFO, "Default values when none are given:");
    Logger::Println(LEVEL::INFO, "\tmin value            = -50");
    Logger::Println(LEVEL::INFO, "\tmax value            = 50");
    Logger::Println(LEVEL::INFO, "\tuniform distribution = 1 (true)\n");
}

void GenerateInstance(const char *filename, int point_count, int min, int max, bool uniform)
{
    vector<Point> points = vector<Point>(point_count);

    // Generates points with even distribution across [min, max]
    if (uniform)
    {
        default_random_engine gen;
        uniform_int_distribution<int> pdis(min, max);
        uniform_int_distribution<int> bdis((int) (1+abs(max-min)*0.05f), (int) (1+abs(max-min)*0.2f));
        for (int i = 0; i < point_count; i++)
        {
            int x = pdis(gen);
            int y = pdis(gen);
            int w = bdis(gen);
            int h = bdis(gen);
            // Add new point to list
            points[i] = Point(x, y, Box(w, h, string("box") + to_string(i+1)));
        }
    }
    else
    {
        srand((unsigned int) time(0));
        int rangeMax = (int) (abs(max-min) * 0.2f) + 1;
        int rangeMin = (int) (abs(max-min) * 0.05f);
        int range = abs(max-min) + 1;

        for (int i = 0; i < point_count; i++)
        {
            int x = min + (rand() % range);
            int y = min + (rand() % range);
            int w = rangeMin + (rand() % rangeMax);
            int h = rangeMin + (rand() % rangeMax);
            // Add new point to list
            points[i] = Point(x, y, Box(w, h, string("box") + to_string(i+1)));
        }
    }

    Instance instance = Instance(&points);
    instance.WriteFile((string(DATA_DIR) + filename).c_str());
}