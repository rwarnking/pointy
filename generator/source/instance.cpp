#include "../header/instance.h"

using namespace std;

Instance::Instance()
{
    points = vector<Point>();
    point_count = 0;
    min_x = max_x = 0;
    min_y = max_y = 0;
}

Instance::Instance(const char* filename, bool read_solution)
{
    min_x = max_x = 0;
    min_y = max_y = 0;
    ReadFile(filename, read_solution);
}

Instance::Instance(string filename, bool read_solution)
{
    min_x = max_x = 0;
    min_y = max_y = 0;
    ReadFile(filename.c_str(), read_solution);
}

Instance::Instance(vector<Point> *p)
{
    points = *p;
    point_count = (int)points.size();
    min_x = max_x = 0;
    min_y = max_y = 0;
}

Instance::~Instance()
{
    points.clear();
}

int Instance::GetPointCount()
{
    return point_count;
}

std::vector<Point>* Instance::GetPoints()
{
    return &points;
}

int Instance::GetRangeX()
{
    return abs(max_x - min_x);
}

int Instance::GetRangeY()
{
    return abs(max_y - min_y);
}

void Instance::SetPointCount(int count)
{
    point_count = count;
}

void Instance::SetPoints(std::vector<Point> *p)
{
    points = *p;
}

void Instance::ReadFile(const char *filename, bool read_solution)
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
            ReadPoints(content.substr(pos + 1, string::npos), read_solution);
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
        cerr << "\t" << e.what() << endl;
    }
}

size_t Instance::ReadPointCount(string content)
{
    size_t pos = content.find("\n");
    try
    {
        point_count = stoi(content.substr(0, pos));
    }
    catch (invalid_argument &e)
    {
        cerr << "Point count not a valid argument" << endl;
        cerr << "\t" << e.what() << endl;
        // TODO exit programm (logger)
    }
    catch (out_of_range &e)
    {
        cerr << "Point count out of range" << endl;
        cerr << "\t" << e.what() << endl;
        // TODO exit programm (logger)
    }

    return pos;
}

void Instance::ReadPoints(string content, bool read_solution)
{
    if (point_count <= 0) return;

    size_t pos = 0;
    points = vector<Point>(point_count);

    for (int i = 0; i < point_count; i++)
    {
        int vals[4] = {};
        for (int j = 0; j < 4; j++)
        {
            pos = content.find(" ");
            try
            {
                vals[j] = stoi(content.substr(0, pos));
                content = content.substr(pos + 1, string::npos);
            }
            catch (invalid_argument &e)
            {
                cerr << "Point " << i << " has invalid data" << endl;
                cerr << "\t" << e.what() << endl;
                // TODO exit programm (logger)
            }
            catch (out_of_range &e)
            {
                cerr << "Point " << i << " has data that is out of range" << endl;
                cerr << "\t" << e.what() << endl;
                // TODO exit programm (logger)
            }
        }
        // Read label
        pos = content.find(" ");
        string label = content.substr(0, pos);
        content = content.substr(pos + 1, string::npos);

        if (read_solution)
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
                    content = content.substr(pos + 1, string::npos);
                }
                catch (invalid_argument &e)
                {
                    cerr << "Point " << i << " has invalid data" << endl;
                    cerr << "\t" << e.what() << endl;
                    // TODO exit programm (logger)
                }
                catch (out_of_range &e)
                {
                    cerr << "Point " << i << " has data that is out of range" << endl;
                    cerr << "\t" << e.what() << endl;
                    // TODO exit programm (logger)
                }
            }
            points[i] = Point(vals[0], vals[1], Box(vals[2], vals[3], box_koords[0], box_koords[1], label));
            points[i].has_box = has_box;
        }
        else
        {
            points[i] = Point(vals[0], vals[1], Box(vals[2], vals[3], label));

            pos = content.find("\n");
            content = content.substr(pos + 1, string::npos);
        }
    }
}

void Instance::WriteFile(const char *filename)
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
        cerr << "\t" << e.what() << endl;
    }
}