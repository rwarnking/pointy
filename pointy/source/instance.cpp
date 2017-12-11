#include "../header/instance.h"

using namespace std;

Box::Box() :
	x(0),
	y(0),
	length(0),
	height(0),
	label(""),
	corner(NONE) {}

Box::Box(int w, int h, std::string s) :
	x(0),
	y(0),
	length(w),
	height(h),
	label(s),
	corner(NONE) {}

Box::Box(int w, int h, int xdim, int ydim, std::string s) :
	x(xdim),
	y(ydim),
	length(w),
	height(h),
	label(s),
	corner(NONE) {}

Box::Box(const Box &other)
{
	x = other.x;
	y = other.y;
	length = other.length;
	height = other.height;
	label = other.label;
	corner = other.corner;
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
	int diffx = x - box.x;
    int diffy = y - box.y;

    if (diffx == 0 && diffy == 0)
        box.corner = TOP_LEFT;
    else if (diffx > 0 && diffy == 0)
        box.corner = TOP_RIGHT;
    else if (diffx == 0 && diffy > 0)
        box.corner = BOT_LEFT;
    else if (diffx < 0 && diffy > 0)
    	box.corner = BOT_RIGHT;
    else
    	box.corner = NONE;

}

std::string Point::ToString()
{
	std::string s = std::to_string(x);
	s += " " + std::to_string(y) + " " + std::to_string(box.length) + " " + std::to_string(box.height) + " " + box.label + " " +
		 std::to_string(HasBox()) + " " + std::to_string(box.x) + " " + std::to_string(box.y) + "\n";

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
	// TODO search min/max
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

int Instance::GetPointCount()
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
	cout << "max x: " << max_x << " min x: " << min_x << " max l: " << max_l << endl;
	cout << "max y: " << max_y << " min y: " << min_y << " max h: " << max_h << endl;

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

		// Check min and max x value
		if (vals[0] > max_x) max_x = vals[0];
		else if (vals[0] < min_x) min_x = vals[0];
		// Check min and max y value
		if (vals[1] > max_y) max_y = vals[1];
		else if (vals[1] < min_y) min_y = vals[1];

		// Check min and max box length
		if (vals[2] > max_l) max_l = vals[2];
		else if (vals[2] < min_l) min_l = vals[2];
		// Check min and max box height
		if (vals[3] > max_h) max_h = vals[3];
		else if (vals[3] < min_h) min_h = vals[3];

		// Read label
		pos = content.find(" ");
		string label = content.substr(0, pos);
		content = content.substr(pos + 1, string::npos);

		if (read_solution)
		{
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
			points[i].ComputeBoxFromCoords();
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