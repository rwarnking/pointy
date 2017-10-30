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

void Problem::CheckSolution(char *infile)
{
	ReadFile(infile);

	// Check solution
	int count = GetBoxCount();
	WriteToConsole();

	// Print evaluation
	cout << count << endl;
}

// Read problem in 'infile', then generate solution(s) and
// write the solution to 'outfile'
void Problem::GenerateSolution(char *infile, char *outfile)
{
	ReadFile(infile);

	// Compute (optimal) solution
	int count = GetBoxCount();

	WriteFile(outfile);
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
	int x = 5, y = 5;

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
				else if (it->has_box && it->box.x <= i && it->box.x + it->box.length >= i && it->box.y <= j && it->box.height - it->box.y >= j)
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