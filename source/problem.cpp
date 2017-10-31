#include "../header/problem.h"

using namespace std;

Problem::Problem()
{
    point_count = 0;
    points = vector<Point>();
}

Problem::~Problem()
{
    points.clear();
    points.shrink_to_fit();
}

// Einlesen
void Problem::ReadFile(char *filename)
{
    try
    {
        ifstream file_reader(filename, ios::binary | ios::ate);
        if (file_reader)
        {
            auto file_size = file_reader.tellg();
            file_reader.seekg(ios::beg);
            string content(file_size, 0);
            file_reader.read(&content[0], file_size);

            // First line containing point count
            size_t pos = ReadPointCount(content);

            // Read all points
            ReadPoints(content.substr(pos+1, string::npos));
        }
        else
        {
            cerr << "Error occurred while trying to read file: " << filename << endl;
            exit(-1);
        }
    }
    catch (exception &e)
    {
        cerr << "Error occurred while trying to read file: " << filename << endl;
    }
}

size_t Problem::ReadPointCount(string content)
{
    size_t pos = content.find("\n");
    try
    {
        point_count = stoi(content.substr(0, pos));
    }
    catch(invalid_argument &e)
    {
        cerr << "Point count not a valid argument" << endl;
        // TODO exit programm (logger)
    }
    catch(out_of_range &e)
    {
        cerr << "Point count out of range" << endl;
        // TODO exit programm (logger)
    }

    return pos;
}

void Problem::ReadPoints(string content, bool solution)
{
    if (point_count <= 0) return;

    size_t pos = 0;
    points.reserve(point_count);

    for(int i = 0; i < point_count; i++)
    {
        int vals[4] = {};
        for (int j = 0; j < 4; j++)
        {
            pos = content.find(" ");
            try
            {
                vals[j] = stoi(content.substr(0, pos));
                content = content.substr(pos+1, string::npos);
            }
            catch(invalid_argument &e)
            {
                cerr << "Point " << i << " has invalid data" << endl;
                // TODO exit programm (logger)
            }
            catch(out_of_range &e)
            {
                cerr << "Point " << i << " has data that is out of range" << endl;
                // TODO exit programm (logger)
            }
        }
        // Read label
        pos = content.find(" ");
        string label = content.substr(0, pos);
        content = content.substr(pos+1, string::npos);

        if (solution)
        {
            bool has_box = content[0] == '1';
            content = content.substr(2, string::npos);

            int box_koords[2] = {};
            for (int j = 0; j < 2; j++)
            {
                pos = content.find(j == 0 ? " " : "\n");
                try
                {
                    box_koords[j] = stoi(content.substr(0, pos));
                    content = content.substr(pos+1, string::npos);
                }
                catch(invalid_argument &e)
                {
                    cerr << "Point " << i << " has invalid data" << endl;
                    // TODO exit programm (logger)
                }
                catch(out_of_range &e)
                {
                    cerr << "Point " << i << " has data that is out of range" << endl;
                    // TODO exit programm (logger)
                }
            }
            points.push_back(Point(vals[0], vals[1], Box(vals[2], vals[3], box_koords[0], box_koords[1], label)));
            points.back().has_box = has_box;
        }
        else
        {
            points.push_back(Point(vals[0], vals[1], Box(vals[2], vals[3], label)));

            pos = content.find("\n");
            content = content.substr(pos+1, string::npos);
        }
    }
}

// Write problem + solution to 'filename'
void Problem::WriteFile(char *filename)
{
    try
    {
        ofstream file_writer(filename, ios::out);
        if (file_writer)
        {
            file_writer << point_count << endl;
            for (auto it = points.begin(); it != points.end(); it++)
            {
                file_writer << it->ToString();
            }
        }
        else
        {
            cerr << "Error opening writing stream" << endl;
        }
    }
    catch (exception &e)
    {
        cerr << "Error occurred while trying to write file: " << filename << endl;
    }
}

void Problem::WritePointCount()
{
}

void Problem::WritePoints(bool solution)
{
}

void Problem::CheckSolution(char *infile)
{
    ReadFile(infile);

    // Check solution
    switch(IsFeasible())
    {
        case 0: cout << GetBoxCount() << endl; break;
        case 1: cout << "ERROR: label boxes overlapped" << endl; break;
        default: cout << "ERROR: unknown problem" << endl;
    }

    //WriteToConsole();
}

// Read problem in 'infile', then generate solution(s) and
// write the solution to 'outfile'
void Problem::GenerateSolution(char *infile, char *outfile)
{
    ReadFile(infile);


    // Save optimal solution


    // Gehe alle Punkte durch
    for (auto it = points.begin(); it != points.end(); it++)
    {
        // Wenn der Punkt bereits eine Box hat, schaue ob sie mit einer anderen Intersected
        if (false && it->has_box)
        {
            bool intersects = false;
            for (auto it2 = points.begin(); it2 != points.end() && !intersects; it2++)
            {
                if (it == it2)
                    continue;

                intersects = Intersects(it->box, it2->box);
            }
            // Wenn sie intersected setze die Box auf false
            if (intersects)
            {
                it->has_box = false;
                if (it != points.begin())
                    it--;// Achtung fehler wenn it == 0
            }
        }
        else
        {
            for (short corner = 0; corner < 4; corner++)
            {
                switch (corner)
                {
                // Ecke links unten
                case 0: it->box.x = it->x; it->box.y = it->y + it->box.height; break;
                // Ecke links oben
                case 1: it->box.x = it->x; it->box.y = it->y; break;
                // Ecke rechts oben
                case 2: it->box.x = it->x - it->box.length; it->box.y = it->y; break;
                // Ecke rechts unten
                case 3: it->box.x = it->x - it->box.length; it->box.y = it->y + it->box.height; break;
                default: cout << "error" << endl; break;
                }

                bool intersects = false;
                for (auto it2 = points.begin(); it2 != points.end() && !intersects; it2++)
                {
                    if (it == it2)
                        continue;

                    if (it2->has_box)
                        intersects = Intersects(it->box, it2->box);
                }
                // Wenn sie intersected setze die Box auf false
                if (!intersects)
                {
                    it->has_box = true;
                    break;
                }
            }
        }
    }

    int count = GetBoxCount();
    double time = 0.0;

    WriteToConsole();
    cout << count << "\t" << time;

    WriteFile(outfile);
}

// Returns true if overlapping
bool Problem::Intersects(Box p1, Box p2)
{
    return p1.x < p2.x + p2.length && p1.x + p1.length > p2.x &&
           p1.y > p2.y - p2.height && p1.y - p1.height < p2.y;
}

bool Problem::Intersects(int one, int two)
{
    return points[one].box.x < points[two].box.x + points[two].box.length && points[one].box.x + points[one].box.length > points[two].box.x &&
           points[one].box.y > points[two].box.y - points[two].box.height && points[one].box.y - points[one].box.height < points[two].box.y;
}

int Problem::IsFeasible()
{
    for (auto it = points.begin(); it != points.end(); it++)
    {
        if (it->has_box)
        {
            for (auto it2 = points.begin(); it2 != points.end(); it2++)
            {
                if (it == it2)
                    continue;
        
                if (it2->has_box && Intersects(it->box, it2->box))
                    return 1;
            }
        }
    }

    return 0;
}

int Problem::GetBoxCount()
{
    int count = 0;
    for (auto it = points.begin(); it != points.end(); it++)
    {
        if (it->has_box)
            count++;
    }
    
    return count;
}

void Problem::WriteToConsole()
{
    int x = 25, y = 25;

    cout << "   |";
    for (int i = -x; i <= x; i++)
    {
        i < 0 ? cout << i : cout << " " << i;
    }
    cout << endl;

    for (int j = y; j >= -y; j--)
    {
        j < 0 ? cout << j << " | " : cout << " " << j << " | " ;
        for (int i = -x; i <= x; i++)
        {
            bool found = false;
            for (auto it = points.begin(); it != points.end(); it++)
            {
                if (it->x == i && it->y == j)
                {
                    cout << "X ";
                    found = true;
                    break;
                }
                else if (it->has_box && it->box.x <= i && it->box.y >= j && it->box.y - it->box.height <= j && it->box.x + it->box.length >= i )
                {
                    cout << "O ";
                    found = true;
                    break;
                }
            }
            if (!found) {
                if (j == 0)
                    i == 0 ? cout << "+-" : cout << "--";
                else if (i == 0)
                    cout << "| ";
                else
                    cout << "  ";
            }
        }
        cout << endl;
    }
}