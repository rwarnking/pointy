#include "../header/instance.h"
#include "../header/logger.h"

#include <exception>
#include <fstream>
#include <cmath>
#include <climits>
#include <algorithm>

using namespace std;
using namespace logger;

Box::Box() : Box(0, 0, "") {}

Box::Box(int w, int h, std::string s) : Box(w, h, 0, 0, s) {}

Box::Box(int w, int h, int xdim, int ydim, std::string s)
{
	x = xdim;
	y = ydim;
	length = w;
	height = h;
	label = s;
	corner = NONE;
}

Box::Box(const Box &other)
{
	x = other.x;
	y = other.y;
	length = other.length;
	height = other.height;
	label = other.label;
	corner = other.corner;
}

void Box::SetCorner(int px, int py, CORNER new_corner)
{
	corner = new_corner;
	switch (corner)
	{
		case BOT_LEFT: x = px; y = py + height; break;
		case TOP_LEFT: x = px; y = py; break;
		case TOP_RIGHT: x = px - length; y = py; break;
		case BOT_RIGHT: x = px - length; y = py + height; break;
		default: x = y = INT_MIN; break;
	}
}

std::string Box::ToString()
{
	std::string s = std::string("x: ");
	s += std::to_string(x) + ", y: " + std::to_string(y) + ", length: " + std::to_string(length) + ", height: " +
		 std::to_string(height) + ", label: " + label + "corner: " + to_string(corner);

	return s;
}

bool Box::Intersects(Box &other)
{
	if (corner == NONE || other.corner == NONE)
		return false;
	else
        return x < other.x + other.length && x + length > other.x &&
		       y > other.y - other.height && y - height < other.y;
}

Point::Point() :
	x(0),
	y(0) {}

Point::Point(int xdim, int ydim, Box b) :
	x(xdim),
	y(ydim) { box = b; }

Point::Point(const Point &other)
{
	x = other.x;
	y = other.y;
	box = Box(other.box);
}

bool Point::HasBox()
{
	return box.corner != NONE;
}

void Point::ComputeBoxFromCoords()
{
	int diffx = abs(x - box.x);
    int diffy = abs(y - box.y);

    if (diffx == 0 && diffy == 0)
        box.corner = TOP_LEFT;
    else if (diffx > 0 && diffy == 0)
        box.corner = TOP_RIGHT;
    else if (diffx == 0 && diffy > 0)
        box.corner = BOT_LEFT;
    else if (diffx > 0 && diffy > 0)
    	box.corner = BOT_RIGHT;
    else
    	box.corner = NONE;

}

std::string Point::ToString()
{
	std::string s = std::to_string(x);
	s += " " + std::to_string(y) + " " + std::to_string(box.length) + " " + std::to_string(box.height) + " " + box.label + " " +
		 std::to_string(HasBox()) + " " + std::to_string(box.x) + " " + std::to_string(box.y);

	return s;
}

Instance::Instance()
{
	points = vector<Point>();
	point_count = 0;
	min_x = min_y = INT_MAX;
	max_x = max_y = INT_MIN;
	min_l = min_h  = INT_MAX;
	max_l = max_h = INT_MIN;
}

Instance::Instance(vector<Point> *p)
{
	points = *p;
	point_count = (int)points.size();
	min_x = min_y = INT_MAX;
	max_x = max_y = INT_MIN;
	min_l = min_h  = INT_MAX;
	max_l = max_h = INT_MIN;
}

Instance::Instance(const char* filename, bool read_solution)
{
	min_x = min_y = INT_MAX;
	max_x = max_y = INT_MIN;
	min_l = min_h  = INT_MAX;
	max_l = max_h = INT_MIN;
	point_count = 0;
	ReadFile(filename, read_solution);
}

Instance::Instance(string filename, bool read_solution)
{
	Instance(filename.c_str(), read_solution);
}

Instance::~Instance()
{
	points.clear();
}

size_t Instance::GetPointCount()
{
	return point_count;
}

std::vector<Point>* Instance::GetPoints()
{
	return &points;
}

int Instance::MinX()
{
	return min_x;
}

int Instance::MaxX()
{
	return max_x;
}

int Instance::MinY()
{
	return min_y;
}

int Instance::MaxY()
{
	return max_y;
}

int Instance::GetDimensionX()
{
	return (max_x - min_x) + max_l * 2;
}

int Instance::GetDimensionY()
{
	return (max_y - min_y) + max_h * 2;
}

int Instance::GetMiddleX()
{
	return min_x + (max_x - min_x) / 2;
}

int Instance::GetMiddleY()
{
	return min_y + (max_y - min_y) / 2;
}

bool Instance::CheckSolution()
{
	for (size_t i = 0; i < points.size(); i++)
	{
		for (size_t j = i+1; j < points.size(); j++)
		{
			if (points[i].box.Intersects(points[j].box))
			{
				// Logger::Println(LEVEL::DEBUG, "\nOverlap:");
				// Logger::Println(LEVEL::DEBUG, points[i].ToString());
				// Logger::Println(LEVEL::DEBUG, points[j].ToString(), "\n");
				return false;
			}
		}
	}
	return true;
}

void Instance::SetPointCount(int count)
{
	point_count = count;
}

void Instance::SetPoints(std::vector<Point> *p)
{
	points = *p;
}

void Instance::SetBox(int index, CORNER corner)
{
	if (index < 0 || index >= (int)points.size())
		return;

	points[index].box.SetCorner(points[index].x, points[index].y, corner);
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
			Logger::Println(LEVEL::ERR, "ERR occurred while trying to read file: ", filename);
		}
	}
	catch (exception)
	{
		Logger::Println(LEVEL::ERR, "ERR occurred while trying to read file: ", filename);
	}
}

size_t Instance::ReadPointCount(string content)
{
	size_t pos = content.find("\n");
	string sub = content.substr(0, pos);

	try
	{
		point_count = stoi(sub);
	}
	catch (invalid_argument)
	{
		Logger::Println(LEVEL::ERR, "Point count not a valid argument: ", sub);
		// TODO exit programm (logger)
	}
	catch (out_of_range)
	{
		Logger::Println(LEVEL::ERR, "Point count out of range: ", sub);
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
			pos = FindSpaceOrTab(content);
			try
			{
				vals[j] = stoi(content.substr(0, pos));
				pos = SkipSpacesOrTabs(content, pos+1);
				content = content.substr(pos, string::npos);
			}
			catch (invalid_argument)
			{
				Logger::Println(LEVEL::ERR, "Point ", i, " has invalid data");
			}
			catch (out_of_range)
			{
				Logger::Println(LEVEL::ERR, "Point ", i, " has data that is out of range");
			}
		}

		// Check min and max x value
		if (vals[0] > max_x) 
			max_x = vals[0];
		if (vals[0] < min_x) 
			min_x = vals[0];
		// Check min and max y value
		if (vals[1] > max_y) 
			max_y = vals[1];
		if (vals[1] < min_y) 
			min_y = vals[1];

		// Check min and max box length
		if (vals[2] > max_l) 
			max_l = vals[2];
		if (vals[2] < min_l) 
			min_l = vals[2];
		// Check min and max box height
		if (vals[3] > max_h) 
			max_h = vals[3];
		if (vals[3] < min_h) 
			min_h = vals[3];

		// Read label
		pos = FindSpaceOrTab(content);
		string label = content.substr(0, pos);
		pos = SkipSpacesOrTabs(content, pos+1);
		content = content.substr(pos, string::npos);

		if (read_solution)
		{
			int box_koords[3] = {};
			for (int j = 0; j < 3; j++)
			{
				pos = j == 2 ? content.find("\n") : FindSpaceOrTab(content);
				try
				{
					box_koords[j] = stoi(content.substr(0, pos));
					//pos = SkipSpacesOrTabs(content, pos+1);
					content = content.substr(pos+1, string::npos);
				}
				catch (invalid_argument)
				{
					Logger::Println(LEVEL::ERR, "Point ", i, " has invalid data");
				}
				catch (out_of_range)
				{
					Logger::Println(LEVEL::ERR, "Point ", i, " has data that is out of range");
				}
			}

			points[i] = Point(vals[0], vals[1], Box(vals[2], vals[3], box_koords[1], box_koords[2], label));
			
			if (box_koords[0] == 1)
				points[i].ComputeBoxFromCoords();
		}
		else
		{
			points[i] = Point(vals[0], vals[1], Box(vals[2], vals[3], label));

			pos = content.find("\n");
			content = content.substr(pos+1, string::npos);
		}
	}
}

size_t Instance::FindSpaceOrTab(std::string content, size_t start)
{
	size_t pos;
	for (pos = start; pos < content.length(); pos++)
	{
		if (content[pos] == ' ' || content[pos] == '\t')
			return pos;
	}
	return string::npos;
}

size_t Instance::SkipSpacesOrTabs(std::string content, size_t start)
{
	size_t pos;
	for (pos = start; pos < content.length(); pos++)
	{
		if (content[pos] != ' ' || content[pos] != '\t')
			return pos;
	}

	return string::npos;
}

void Instance::WriteFile(const char *filename)
{
	try
	{
		ofstream file_writer(filename, ios::out);
		if (file_writer)
		{
			file_writer << point_count << endl;
			for (auto point : points)
			{
				file_writer << point.ToString() << endl;
			}
			file_writer.close();
		}
		else
		{
			Logger::Println(LEVEL::ERR, "ERR opening writing stream");
		}
	}
	catch (exception)
	{
		Logger::Println(LEVEL::ERR, "ERR occurred while trying to write file: ", filename);
	}
}