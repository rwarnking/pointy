#include "../header/problem.h"

using namespace std;

Problem::Problem()
{
	point_count = 0;
	points = vector<Point>();
	opt = vector<Point>();
}

Problem::~Problem()
{
	points.clear();
	points.shrink_to_fit();

	opt.clear();
	opt.shrink_to_fit();
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
			ReadPoints(content.substr(pos + 1, string::npos), false);
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

size_t Problem::ReadPointCount(string content)
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

void Problem::ReadPoints(string content, bool solution)
{
	if (point_count <= 0) return;

	size_t pos = 0;
	points.reserve(point_count);

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
			points.push_back(Point(vals[0], vals[1], Box(vals[2], vals[3], box_koords[0], box_koords[1], label)));
			points.back().has_box = has_box;
		}
		else
		{
			points.push_back(Point(vals[0], vals[1], Box(vals[2], vals[3], label)));

			pos = content.find("\n");
			content = content.substr(pos + 1, string::npos);
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
		cerr << "\t" << e.what() << endl;
	}
}

void Problem::CheckSolution(char *infile)
{
	ReadFile(infile);

	// Check solution
	switch (IsFeasible())
	{
	case 0: cout << GetBoxCount() << endl; break;
	case 1: cout << "ERROR: label boxes overlapped" << endl; break;
	default: cout << "ERROR: unknown problem" << endl;
	}

	//WriteToConsole();
}

// Read problem in 'infile', then generate solution(s) and
// write the solution to 'outfile'
void Problem::GenerateSolution(char *infile, char *outfile, bool print)
{
	ReadFile(infile);

	// Take time
	chrono::steady_clock::time_point start = chrono::steady_clock::now();

	// Sort from smallest to biggest box
	sort(points.begin(), points.end(), [](Point &p1, Point &p2) { return p1.box.length * p1.box.height < p2.box.length * p2.box.height; });

	//Generator();
	//points = vector<Point>(opt);
	GeneratorArray();

	// Take time
	chrono::steady_clock::time_point end = chrono::steady_clock::now();

	//WriteToConsole();
	cout << GetBoxCount(1) << "\t" << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;

	if (print) 
		WriteToBMP(outfile);

	WriteFile(outfile);
}

bool Problem::Generator(int index)
{
	int tmp1 = GetBoxCount();
	int tmp2 = GetBoxCount(1);

	// Wenn alle Punkte durchgegangen, oder wir generell ein optimum gefunden haben oder wenn dieser Weg kein Optimum mehr werden kann
	if (index == points.size() || tmp2 == point_count || tmp1 + point_count - index < tmp2)
	{
		if (tmp1 > tmp2)
			opt = vector<Point>(points);

		return false;
	}

	auto p = points.begin() + index;

	for (short corner = 0; corner < 5; corner++)
	{
		switch (corner)
		{
			// Ecke links unten
		case 0: p->box.x = p->x; p->box.y = p->y + p->box.height; break;
			// Ecke links oben
		case 1: p->box.x = p->x; p->box.y = p->y; break;
			// Ecke rechts oben
		case 2: p->box.x = p->x - p->box.length; p->box.y = p->y; break;
			// Ecke rechts unten
		case 3: p->box.x = p->x - p->box.length; p->box.y = p->y + p->box.height; break;
		case 4: p->has_box = false; return Generator(index + 1);
		default: cout << "error" << endl; break;
		}

		bool intersects = false;
		for (auto it2 = points.begin(); it2 != points.end() && !intersects; it2++)
		{
			if (p == it2 || !it2->has_box)
				continue;

			intersects = Intersects(p->box, it2->box);
		}
		// Wenn sie intersected setze die Box auf false
		if (!intersects)
		{
			p->has_box = true;
			if (Generator(index + 1))
				return true;
		}
	}
	return false;
}

bool Problem::GeneratorArray(int index)
{
	// Loop through all Elements and mark the boxpositions
	int dpi = 72;
	int width = 500;
	int height = 500;
	int n = width * height;
	Color *pixels = new Color[n];
	int element = 0;
	int r, g, b;
	int middle_x = width / 2;
	int middle_y = height / 2;
	int multiplier = 10;
	int point_size = 5;
	int intensity = 30;

	for (auto p = points.begin(); p != points.end(); p++)
	{
		int l = p->box.length;
		int h = p->box.height;
		for (int x = (p->x - l) * multiplier + middle_x; x < (p->x + l) * multiplier + middle_x; x++) {
			for (int y = (p->y - h) * multiplier + middle_y; y < (p->y + h) * multiplier + middle_y; y++) {

				element = y * width + x;
				pixels[element].g = (pixels[element].g*255 + intensity) / 255;
			}
		}
	}

	// Loop over all points and select the box that has no overlaps
	for (auto p = points.begin(); p != points.end(); p++)
	{
		// Skip element if it already has a box
		if (p->has_box)
			continue;

		for (short corner = 0; corner < 4; corner++)
		{
			switch (corner)
			{
			// Lower left corner
			case 0:
			{
				if (CheckBox(pixels, p->x, p->y, 0, p->box.length, p->box.height, 0, intensity)) {

					int l = p->box.length;
					int h = p->box.height;
					for (int x = (p->x - l) * multiplier + middle_x; x < (p->x + l) * multiplier + middle_x; x++) {
						for (int y = (p->y - h) * multiplier + middle_y; y < (p->y + h) * multiplier + middle_y; y++) {

							element = y * width + x;
							pixels[element].g = (pixels[element].g * 255 - intensity) / 255;
						}
					}

					p->has_box = true;
					p->box.x = p->x;
					p->box.y = p->y + p->box.height;
					corner = 5;
					p = points.begin();
				}
				break;
			}
			// Upper left corner
			case 1:
			{
				if (CheckBox(pixels, p->x, p->y, 0, p->box.length, 0, p->box.height, intensity)) {

					int l = p->box.length;
					int h = p->box.height;
					for (int x = (p->x - l) * multiplier + middle_x; x < (p->x + l) * multiplier + middle_x; x++) {
						for (int y = (p->y - h) * multiplier + middle_y; y < (p->y + h) * multiplier + middle_y; y++) {

							element = y * width + x;
							pixels[element].g = (pixels[element].g * 255 - intensity) / 255;
						}
					}

					p->has_box = true;
					p->box.x = p->x;
					p->box.y = p->y;
					corner = 5;
					p = points.begin();
				}
				break;
			}
			// Upper right corner
			case 2:
			{
				if (CheckBox(pixels, p->x, p->y, p->box.length, 0, 0, p->box.height, intensity)) {

					int l = p->box.length;
					int h = p->box.height;
					for (int x = (p->x - l) * multiplier + middle_x; x < (p->x + l) * multiplier + middle_x; x++) {
						for (int y = (p->y - h) * multiplier + middle_y; y < (p->y + h) * multiplier + middle_y; y++) {

							element = y * width + x;
							pixels[element].g = (pixels[element].g * 255 - intensity) / 255;
						}
					}

					p->has_box = true;
					p->box.x = p->x - p->box.length;
					p->box.y = p->y;
					corner = 5;
					p = points.begin();
				}
				break;
			}
			// Lower left corner
			case 3:
			{
				if (CheckBox(pixels, p->x, p->y, p->box.length, 0, p->box.height, 0, intensity)) {

					int l = p->box.length;
					int h = p->box.height;
					for (int x = (p->x - l) * multiplier + middle_x; x < (p->x + l) * multiplier + middle_x; x++) {
						for (int y = (p->y - h) * multiplier + middle_y; y < (p->y + h) * multiplier + middle_y; y++) {

							element = y * width + x;
							pixels[element].g = (pixels[element].g * 255 - intensity) / 255;
						}
					}

					p->has_box = true;
					p->box.x = p->x - p->box.length;
					p->box.y = p->y + p->box.height;
					corner = 5;
					p = points.begin();
				}
				break;
			}
			default: cout << "ERROR: switch case 1 " << endl; break;
			}
		}
	}

	int count = 0;
	// Loop over all points and select for each point the box with the least overlaps
	for (auto p = points.begin(); p != points.end(); p++)
	{
		// Skip if element already has a box
		if (p->has_box)
			continue;

		float min;
		float tmp = 50000;
		short corner_s = 0;

		for (short corner = 0; corner < 4; corner++)
		{
			switch (corner)
			{
			// Ecke links unten
			case 0:
			{
				min = CalculateOverlap(pixels, p->x, p->y, 0, p->box.length, p->box.height, 0, intensity);
				break;
			}
			// Ecke links oben
			case 1:
			{
				tmp = CalculateOverlap(pixels, p->x, p->y, 0, p->box.length, 0, p->box.height, intensity);
				if (min > tmp)
				{
					min = tmp;
					corner_s = 1;
				}
				break;
			}
			// Ecke rechts oben
			case 2:
			{
				tmp = CalculateOverlap(pixels, p->x, p->y, p->box.length, 0, 0, p->box.height, intensity);
				if (min > tmp)
				{
					min = tmp;
					corner_s = 2;
				}
				break;
			}
			// Ecke rechts unten
			case 3:
			{
				tmp = CalculateOverlap(pixels, p->x, p->y, p->box.length, 0, p->box.height, 0, intensity);
				if (min > tmp)
				{
					min = tmp;
					corner_s = 3;
				}
				break;
			}
			default: cout << "ERROR: switch case 2" << endl; break;
			}

			if (DEBUG)
				cout << min << " tmp " << tmp << endl;
		}

		// TODO wiederhole das wenn du keine overlaps hast nem den ding
		if (min < 200)
		{
			p->has_box = true;

			switch (corner_s)
			{
				// Ecke links unten
			case 0:
			{
				p->box.x = p->x;
				p->box.y = p->y + p->box.height;
				break;
			}
			// Ecke links oben
			case 1:
			{
				p->box.x = p->x;
				p->box.y = p->y;
				break;
			}
			// Ecke rechts oben
			case 2:
			{
				p->box.x = p->x - p->box.length;
				p->box.y = p->y;
				break;
			}
			// Ecke rechts unten
			case 3:
			{
				p->box.x = p->x - p->box.length;
				p->box.y = p->y + p->box.height;
				break;
			}
			default: cout << "error" << endl; break;
			}
		}

		if (DEBUG)
			savebmp(("tmp_solution" + std::to_string(count) +".bmp").c_str(), width, height, dpi, pixels);

		int l = p->box.length;
		int h = p->box.height;
		for (int x = (p->x - l) * multiplier + middle_x; x < (p->x + l) * multiplier + middle_x; x++) {
			for (int y = (p->y - h) * multiplier + middle_y; y < (p->y + h) * multiplier + middle_y; y++) {

				element = y * width + x;
				if (pixels[element].g != 1.0f)
					pixels[element].g = (pixels[element].g * 255 - intensity) / 255;
			}
		}

		if (min < 200)
		for (int x = p->box.x * multiplier + middle_x; x < (p->box.x + p->box.length) * multiplier + middle_x; x++) {
			for (int y = (p->box.y - p->box.height) * multiplier + middle_y; y < p->box.y * multiplier + middle_y; y++) {

				element = y * width + x;
				pixels[element].g = 1.0f;
			}
		}

		count++;
	}

	opt = vector<Point>(points);

	delete[] pixels;

	return false;
}

// TODO einfach den Point mitgeben
bool Problem::CheckBox(Color* pixels, int px, int py, int ll, int lr, int ho, int hu, int intensity)
{
	int multiplier = 25;
	int width = 500;
	int middle_x = 250;
	int middle_y = 250;
	int element;


	for (int x = (px - ll) * multiplier + middle_x; x < (px + lr) * multiplier + middle_x; x++) {
		for (int y = (py - hu) * multiplier + middle_y; y < (py + ho) * multiplier + middle_y; y++) {

			element = y * width + x;
			if (pixels[element].g != intensity / 255.0)
			{
				return false;
			}
		}
	}
	return true;
}

float Problem::CalculateOverlap(Color* pixels, int px, int py, int ll, int lr, int ho, int hu, int intensity)
{
	int multiplier = 10;
	int width = 500;
	int middle_x = 250;
	int middle_y = 250;
	int element;
	float amount = 0.0;

	// THEORETISCH soll hier der flächeninhalt errechnet werden
	// soll heißen
	// gesamtfärbung / flächeninhalt  = durchschnittliche Faerbung

	for (int x = (px - ll) * multiplier + middle_x; x < (px + lr) * multiplier + middle_x; x++) {
		for (int y = (py - hu) * multiplier + middle_y; y < (py + ho) * multiplier + middle_y; y++) {
			element = y * width + x;
			if (pixels[element].g == 1.0f)
				return 510.0f;
			amount += pixels[element].g * 255.0f;
		}
	}
	return amount / (((px + lr) * multiplier + middle_x - (px - ll) * multiplier + middle_x) * ((py + ho) * multiplier + middle_y - (py - hu) * multiplier + middle_y));
}

void Problem::Clean(Color* pixels)
{
	int multiplier = 10;
	int width = 500;
	int middle_x = 250;
	int middle_y = 250;
	int element;
	int intensity = 30;

	for (auto p = points.begin(); p != points.end(); p++)
	{
		if (!p->has_box)
			continue;

		int l = p->box.length;
		int h = p->box.height;
		for (int x = (p->x - l) * multiplier + middle_x; x < (p->x + l) * multiplier + middle_x; x++) {
			for (int y = (p->y - h) * multiplier + middle_y; y < (p->y + h) * multiplier + middle_y; y++) {

				element = y * width + x;
				if (pixels[element].g != 1.0f)
					pixels[element].g = (pixels[element].g * 255 - intensity) / 255;
			}
		}
	}
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
	int prev = 1;
	for (auto it = points.begin(); it != points.end(); it++, prev++)
	{
		if (it->has_box)
		{
			for (auto it2 = points.begin() + prev; it2 != points.end(); it2++)
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

int Problem::GetBoxCount(short which)
{
	int count = 0;
	vector<Point> work = which == 0 ? points : opt;
	for (auto it = work.begin(); it != work.end(); it++)
	{
		if (it->has_box)
			count++;
	}

	return count;
}

void Problem::WriteToConsole()
{
	int x = 9, y = 9;

	cout << "   |";
	for (int i = -x; i <= x; i++)
	{
		i < 0 ? cout << i : cout << " " << i;
	}
	cout << endl;

	for (int j = y; j >= -y; j--)
	{
		j < 0 ? cout << j << " | " : cout << " " << j << " | ";
		for (int i = -x; i <= x; i++)
		{
			bool found = false;
			for (auto it = opt.begin(); it != opt.end(); it++)
			{
				if (it->x == i && it->y == j)
				{
					cout << "X ";
					found = true;
					break;
				}
				else if (it->has_box && it->box.x <= i && it->box.y >= j && it->box.y - it->box.height <= j && it->box.x + it->box.length >= i)
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

void Problem::WriteToBMP(char *filename)
{
	int dpi = 72;
	int width = 500;
	int height = 500;
	int n = width * height;
	Color *pixels = new Color[n];
	int element;
	int r, g, b;
	int middle_x = width / 2;
	int middle_y = height / 2;
	int multiplier = 25;
	int point_size = 5;

	// Create completely white image
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			// Element position in pixels array
			element = y * width + x;

			r = 255;
			g = 255;
			b = 255;

			pixels[element] = Color(r, g, b);
		}
	}

	// Print box border and content
	for (auto p = points.begin(); p != points.end(); p++)
	{
		if (!p->has_box)
			continue;

		int l = p->box.length;
		int h = p->box.height;
		for (int x = p->box.x * multiplier + middle_x; x <= (p->box.x + p->box.length) * multiplier + middle_x; x++) {
			for (int y = (p->box.y - p->box.height) * multiplier + middle_y; y <= p->box.y * multiplier + middle_y; y++) {

				element = y * width + x;

				if (p->box.x * multiplier == x - middle_x ||
					p->box.y * multiplier == y - middle_y ||
					(p->box.y - p->box.height) * multiplier == y - middle_y ||
					(p->box.x + p->box.length) * multiplier == x - middle_x)
				{
					g = 0;
					r = 0;
					b = 0;
					pixels[element] = Color(r, g, b);
					continue;
				}

				g = 255;
				r = 0;
				b = 0;
				pixels[element] = Color(r, g, b);
			}
		}
	}

	// Print box points
	for (auto it = opt.begin(); it != opt.end(); it++)
	{
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				// Element position in pixels array
				element = y * width + x;

				if (it->x * multiplier - point_size <= x - middle_x &&
					it->x * multiplier + point_size >= x - middle_x &&
					it->y * multiplier + point_size >= y - middle_y &&
					it->y * multiplier - point_size <= y - middle_y)
				{
					r = 255;
					g = 0;
					b = 0;
					pixels[element] = Color(r, g, b);
					continue;
				}
			}
		}
	}

	string file = string(filename);
	string::size_type pos = file.find_last_of(".");

	savebmp((file.substr(0, pos) + "_solution.bmp").c_str(), width, height, dpi, pixels);

	delete[] pixels;
}