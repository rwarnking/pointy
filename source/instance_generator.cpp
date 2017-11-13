#include "../header/instance_generator.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        try
        {
            int points = stoi(argv[2]);
            GenerateInstance(argv[1], points);
        }
        catch(exception &e)
        {
            cerr << "ERROR: invalid point count" << endl;
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
                cerr << "ERROR: min value must be less or equal to max value" << endl;
                PrintHelp();
                return -1;
            }

            bool even = true;
            if (argc == 6) 
                even = stoi(argv[5]) == 1;

            GenerateInstance(argv[1], points, min, max, even);
        }
        catch(exception &e)
        {
            cerr << "ERROR: invalid point count" << endl;
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
    cout << endl << "This program generates a problem instance for the 4-corner labelling problem" << endl << endl;
    cout << "Usage: instance_generator <filename> <number of points> [min value] [max value] [uniform distribution{0=false,1=true}]" << endl << endl;
    cout << "Default values when none are given:" << endl;
    cout << "\tmin value            = -50" << endl;
    cout << "\tmax value            = 50" << endl;
    cout << "\tuniform distribution = 1 (true)" << endl << endl;
}

void GenerateInstance(const char *filename, int point_count, int min, int max, bool uniform)
{
    vector<Point> points = vector<Point>(point_count);

    // Generates points with even distribution across [min, max]
    if (uniform)
    {
        default_random_engine gen;
        uniform_int_distribution<int> pdis(min, max);
        uniform_int_distribution<int> bdis(1+abs(max-min)*0.05, 1+abs(max-min)*0.2);
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
        srand(time(0));
        for (int i = 0; i < point_count; i++)
        {
            int rangeMax = abs(max-min) * 0.2f;
            int rangeMin = abs(max-min) * 0.05f;

            int x = rand() % max + min;
            int y = rand() % max + min;
            int w = rand() % rangeMax + rangeMin + 1;
            int h = rand() % rangeMax + rangeMin + 1;
            // Add new point to list
            points[i] = Point(x, y, Box(w, h, string("box") + to_string(i+1)));
        }
    }

    Instance instance = Instance(&points);
    instance.WriteFile(filename);
}