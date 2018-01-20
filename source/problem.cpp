#include "../header/problem.h"
#include "../header/bitmap.h"
#include "../header/simulated_annealing.h"
#include "../header/logger.h"

#include <string>
#include <algorithm>
#include <chrono>
#include <cfloat>

using namespace std;
using namespace logger;

Problem::Problem()
{
	instance = new Instance();
}

Problem::Problem(char *instance_file)
{
	instance = new Instance(instance_file);
}

Problem::~Problem()
{
	delete instance;
}

void Problem::CheckSolution(char *infile)
{
	instance = new Instance(infile, true);

	// Check solution
	switch (IsFeasible())
	{
		case 0: Logger::Println(LEVEL::INFO, GetBoxCount(), " out of ", instance->GetPointCount()); break;
		case 1: Logger::Println(LEVEL::ERR, "ERR: found overlapping labels"); break;
		default: Logger::Println(LEVEL::ERR, "ERR: unknown problem");
	}
}

size_t Problem::Solve(ALGORITHM algorithm)
{
	// Sort from smallest to biggest box
	sort(instance->points.begin(), instance->points.end(), [](Point &p1, Point &p2) { return p1.box.length * p1.box.height < p2.box.length * p2.box.height; });

	size_t objective_value = 0;
	switch (algorithm)
	{
		case IDIOT:
		{
			SimpleSolve();
			objective_value = GetBoxCount(algorithm);
		} break;
		case GRAPHIC:
		{
			// TODO: different class/file
			GeneratorArray();
			objective_value = GetBoxCount(algorithm);
		} break;
		case SIMULATED_ANNEALING:
		{
			SimulatedAnnealing solver = SimulatedAnnealing();
			solver.SetIterations(min(instance->GetPointCount()*1000, (size_t)500000));
			
			// Config 1
			// solver.SetRandomMove(false);
			// solver.SetTabuList(true);
			// solver.SetRandomStart(false);
			
			// Config 2
			solver.SetRandomMove(true);
			solver.SetTabuList(false);
			solver.SetRandomStart(true);

			objective_value = solver.Solve(instance);
		} break;
		default: Logger::Println(LEVEL::ERR, "ERROR: Invalid algorithm parameter");
	}
	
	return objective_value;
}

// Read problem in 'infile', then generate solution(s) and
// write the solution to 'outfile'
void Problem::GenerateSolution(char *infile, char *outfile, bool print, ALGORITHM algorithm)
{
	instance = new Instance(infile);

	// Take time
	chrono::steady_clock::time_point start = chrono::steady_clock::now();

	size_t objective_value = Solve(algorithm);

	// Take time
	chrono::steady_clock::time_point end = chrono::steady_clock::now();

	//WriteToConsole();
	Logger::Println(LEVEL::INFO, objective_value, "\t", chrono::duration_cast<chrono::milliseconds>(end - start).count(), " ms");

	if (print)
		WriteToBMP(outfile);

	instance->WriteFile(outfile);
}

void Problem::SimpleSolve()
{
	vector<Point>& points = *instance->GetPoints();

	for (size_t i = 0; i < points.size(); i++)
	{
		points[i].box.SetCorner(points[i].x, points[i].y, GetBestOrientation(points[i].x, points[i].y));
		for (size_t j = 0; j < i; j++)
		{
			if (Intersects(points[i].box, points[j].box))
			{
				points[i].box.corner = NONE;
				break;
			}
		}
	}
}

CORNER Problem::GetBestOrientation(int x, int y)
{
	int middle_x = instance->GetMiddleX();
	int middle_y = instance->GetMiddleY();

	if (x <= middle_x)
		return y < middle_y ? TOP_RIGHT : BOT_RIGHT;
	else
		return y < middle_y ? TOP_LEFT : BOT_LEFT;
}

bool Problem::Generator(int index)
{
	size_t point_count = instance->GetPointCount();

	int tmp1 = GetBoxCount();
	int tmp2 = GetBoxCount(GRAPHIC);

	// Wenn alle Punkte durchgegangen, oder wir generell ein optimum gefunden haben oder wenn dieser Weg kein Optimum mehr werden kann
	if (index == instance->points.size() || tmp2 == point_count || tmp1 + point_count - index < tmp2)
	{
		if (tmp1 > tmp2)
			opt = vector<Point>(instance->points);

		return false;
	}

	auto p = instance->points.begin() + index;

	for (int corner = 0; corner < 5; corner++)
	{
		switch (corner)
		{
			// Lower left corner
			case BOT_LEFT: p->box.x = p->x; p->box.y = p->y + p->box.height; break;
			// Upper left corner
			case TOP_LEFT: p->box.x = p->x; p->box.y = p->y; break;
			// Upper right corner
			case TOP_RIGHT: p->box.x = p->x - p->box.length; p->box.y = p->y; break;
			// Lower right corner
			case BOT_RIGHT: p->box.x = p->x - p->box.length; p->box.y = p->y + p->box.height; break;
			case NONE: p->box.corner = NONE; return Generator(index + 1);
			default: Logger::Println(LEVEL::ERR, "ERR ocurred when assigning corner: ", corner); break;
		}

		bool intersects = false;
		for (auto it2 = instance->points.begin(); it2 != instance->points.end() && !intersects; it2++)
		{
			if (p == it2 || !it2->HasBox())
				continue;

			intersects = Intersects(p->box, it2->box);
		}
		// Wenn sie intersected setze die Box auf false
		if (!intersects)
		{
			p->box.corner = (CORNER) corner;
			if (Generator(index + 1))
				return true;
		}
	}
	return false;
}

bool Problem::GeneratorArray(int index)
{
	// set the range of the data 
	data_w = min(instance->GetDimensionX(), (int) sqrt(INT_MAX));
	data_h = min(instance->GetDimensionY(), (int) sqrt(INT_MAX));

	if (data_w * data_h > GetRAM() / (2 * sizeof(int)))
		return false; // TODO verwenden bzw behandeln

	Translate();

	if (Logger::LogLevel() == DEBUG || true)
		SetMultiplier();

	// Loop through all Elements and mark the boxpositions
	int n = data_w * data_h;
	int *pixels = new int[n];
	int element = 0;
	int middle_x = data_w / 2;
	int middle_y = data_h / 2;

	// TODO geht das besser ?
	for (int i = 0; i < n; i++)
		pixels[i] = 0;

	// Color the area
	for (auto p = instance->points.begin(); p != instance->points.end(); p++)
	{
		int l = p->box.length;
		int h = p->box.height;
		for (int x = (p->x - l) + middle_x; x < (p->x + l) + middle_x; x++) {
			for (int y = (p->y - h) + middle_y; y < (p->y + h) + middle_y; y++) {

				element = y * data_w + x;
				pixels[element] = pixels[element] + intensity;
			}
		}
	}

	// Loop over all instance->points and select the boxes wich have no overlaps
	for (auto p = instance->points.begin(); p != instance->points.end(); p++)
	{
		// Skip element if it already has a box
		if (p->HasBox())
			continue;

		bool found = false;
		for (short corner = 0; corner < 4 && !found; corner++)
		{
			switch (corner)
			{
			// Lower left corner
			case BOT_LEFT:
			{
				if (CheckBox(pixels, p->x, p->y, 0, p->box.length, p->box.height, 0)) {
					p->box.x = p->x;
					p->box.y = p->y + p->box.height;
					found = true;
				}
				break;
			}
			// Upper left corner
			case TOP_LEFT:
			{
				if (CheckBox(pixels, p->x, p->y, 0, p->box.length, 0, p->box.height)) {
					p->box.x = p->x;
					p->box.y = p->y;
					found = true;
				}
				break;
			}
			// Upper right corner
			case TOP_RIGHT:
			{
				if (CheckBox(pixels, p->x, p->y, p->box.length, 0, 0, p->box.height)) {
					p->box.x = p->x - p->box.length;
					p->box.y = p->y;
					found = true;
				}
				break;
			}
			// Lower right corner
			case BOT_RIGHT:
			{
				if (CheckBox(pixels, p->x, p->y, p->box.length, 0, p->box.height, 0)) {

					p->box.x = p->x - p->box.length;
					p->box.y = p->y + p->box.height;
					found = true;
				}
				break;
			}
			default: Logger::Println(LEVEL::ERR, "ERR: switch case 1 "); break;
			}

			if (found)
			{
				CleanPointArea(*p, pixels);
				p->box.corner = (CORNER) corner;
				p = instance->points.begin();
			}
		}
	}

	int count = 0;
	// Loop over all instance->points and select for each point the box with the least overlaps
	for (auto p = instance->points.begin(); p != instance->points.end(); p++, count++)
	{
		// Skip if element already has a box
		if (p->HasBox())
			continue;

		double min;
		double tmp = DBL_MAX;
		short corner_s = 0;

		for (short corner = 0; corner < 4; corner++)
		{
			switch (corner)
			{
			// Lower left corner
			case BOT_LEFT:
			{
				min = CalculateOverlap(pixels, p->x, p->y, 0, p->box.length, p->box.height, 0);
				break;
			}
			// Upper left corner
			case TOP_LEFT:
			{
				tmp = CalculateOverlap(pixels, p->x, p->y, 0, p->box.length, 0, p->box.height);
				break;
			}
			// Upper right corner
			case TOP_RIGHT:
			{
				tmp = CalculateOverlap(pixels, p->x, p->y, p->box.length, 0, 0, p->box.height);
				break;
			}
			// Lower right corner
			case BOT_RIGHT:
			{
				tmp = CalculateOverlap(pixels, p->x, p->y, p->box.length, 0, p->box.height, 0);
				break;
			}
			default: Logger::Println(LEVEL::ERR, "ERR: switch case 2"); break;
			}

			// Select the corner if we got a new minimum and the minimum is not a already marked (-1) field
			if ((min > tmp || min == -1) && tmp != -1)
			{
				min = tmp;
				corner_s = corner;
			}
		}

		// TODO wiederhole den Schritt in dem man testet ob ein Kasten vorliegt ohne Overlaps
		if (min != -1 && min < DBL_MAX)
		{
			p->box.corner = (CORNER) corner_s;

			switch (corner_s)
			{
			// Lower left corner
			case BOT_LEFT:
			{
				p->box.x = p->x;
				p->box.y = p->y + p->box.height;
				break;
			}
			// Upper left corner
			case TOP_LEFT:
			{
				p->box.x = p->x;
				p->box.y = p->y;
				break;
			}
			// Upper right corner
			case TOP_RIGHT:
			{
				p->box.x = p->x - p->box.length;
				p->box.y = p->y;
				break;
			}
			// Lower right corner
			case BOT_RIGHT:
			{
				p->box.x = p->x - p->box.length;
				p->box.y = p->y + p->box.height;
				break;
			}
			default: Logger::Println(LEVEL::ERR, "ERR"); break;
			}
		}

		if (Logger::LogLevel() == DEBUG)
			WriteToBMP(count, pixels);

		// Clean up the boxes of this point
		CleanPointArea(*p, pixels);

		// Set the selected Box of this point to -1 to indicate that he is the choosen one
		if (min != -1 && min < DBL_MAX)
		{
			for (int x = p->box.x + middle_x; x < (p->box.x + p->box.length) + middle_x; x++) {
				for (int y = (p->box.y - p->box.height) + middle_y; y < p->box.y + middle_y; y++) {

					element = y * data_w + x;
					pixels[element] = -1;
				}
			}
		}
	}

	if (Logger::LogLevel() == DEBUG)
		WriteToBMP(count, pixels);

	Translate(true);

	opt = vector<Point>(instance->points);

	delete[] pixels;

	return false;
}

void Problem::Translate(bool reverse)
{
	vector<Point>& points = *instance->GetPoints();

	int x_distance = instance->GetMiddleX();
	int y_distance = instance->GetMiddleY();

	for (int i = 0; i < points.size(); i++)
	{
		if (reverse)
		{
			points[i].x += x_distance;
			points[i].y += y_distance;
			points[i].box.x += x_distance;
			points[i].box.y += y_distance;
		}
		else
		{
			points[i].x -= x_distance;
			points[i].y -= y_distance;
		}
	}
}

// TODO einfach den Point mitgeben
bool Problem::CheckBox(int* pixels, int px, int py, int ll, int lr, int ho, int hu)
{
	int middle_x = data_w / 2;
	int middle_y = data_h / 2;
	int element;

	for (int x = (px - ll) + middle_x; x < (px + lr) + middle_x; x++) {
		for (int y = (py - hu) + middle_y; y < (py + ho) + middle_y; y++) {

			element = y * data_w + x;
			if (pixels[element] != intensity)
			{
				return false;
			}
		}
	}
	return true;
}

double Problem::CalculateOverlap(int* pixels, int px, int py, int ll, int lr, int ho, int hu)
{
	int middle_x = data_w / 2;
	int middle_y = data_h / 2;
	int element;
	double amount = 0.0;

	// THEORETISCH soll hier der flaecheninhalt errechnet werden
	// soll heissen
	// gesamtfaerbung / flaecheninhalt  = durchschnittliche Faerbung

	for (int x = (px - ll) + middle_x; x < (px + lr) + middle_x; x++) {
		for (int y = (py - hu) + middle_y; y < (py + ho) + middle_y; y++) {

			element = y * data_w + x;
			if (pixels[element] == -1)
				return -1.0;
			amount += pixels[element];
		}
	}
	return amount / (((px + lr) + middle_x - (px - ll) + middle_x) * ((py + ho) + middle_y - (py - hu) + middle_y));
}

void Problem::CleanPointArea(Point p, int* pixels)
{
	int middle_x = data_w / 2;
	int middle_y = data_h / 2;
	int element;

	int l = p.box.length;
	int h = p.box.height;
	for (int x = (p.x - l) + middle_x; x < (p.x + l) + middle_x; x++) {
		for (int y = (p.y - h) + middle_y; y < (p.y + h) + middle_y; y++) {

			element = y * data_w + x;
			if (pixels[element] != -1)
				pixels[element] = pixels[element] - intensity;
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
	return instance->points[one].box.x < instance->points[two].box.x + instance->points[two].box.length && instance->points[one].box.x + instance->points[one].box.length > instance->points[two].box.x &&
		   instance->points[one].box.y > instance->points[two].box.y - instance->points[two].box.height && instance->points[one].box.y - instance->points[one].box.height < instance->points[two].box.y;
}

int Problem::IsFeasible()
{
	vector<Point>& points = *instance->GetPoints();
	
	for (size_t i = 0; i < points.size(); i++)
	{
		if (points[i].HasBox())
		{
			for (size_t j = i+1; j < points.size(); j++)
			{
				if (points[j].HasBox() && points[i].box.Intersects(points[j].box))
					return 1;
			}
		}
	}

	return 0;
}

int Problem::GetBoxCount(ALGORITHM which)
{
	int count = 0;
	for (auto it = instance->points.begin(); it != instance->points.end(); it++)
	{
		if (it->HasBox())
			count++;
	}

	return count;
}

void Problem::SetMultiplier()
{
	//image_w = data_w;
	//image_h = data_h;

	if ((float)image_w / (float)data_w < 1.0f)
		multiplier = min((float)image_w / (float)data_w, (float)image_h / (float)data_h);
	else
		multiplier = max((float)image_w / (float)data_w, (float)image_h / (float)data_h);

	multiplier = (float)image_w / (float)data_w;

	if (multiplier > 1.0f)
		multiplier = floor(multiplier);
}

void Problem::WriteToBMP(int count, int *data)
{
	int dpi = 72;
	unsigned char *pixels = new unsigned char[image_w * image_h * 3];
	float screenaspect = 2.0f / ((float)image_w / (float)data_w);
	int element;
	int element2;
	int middle_x = data_w / 2;
	int middle_y = data_h / 2;

	std::fill(pixels, pixels + image_w * image_h * 3, 0);

	float multiplier_w = (float)data_w / (float)image_w;
	float multiplier_h = (float)data_h / (float)image_h;

	for (int x = 0; x < image_w; x++) {
		for (int y = 0; y < image_h; y++) {
			// Element position in data array
			element2 = y * image_w * 3 + x * 3;

			if (element2 + 3 > image_w * image_h * 3 || element2 < 0)
				continue;

			element = (int) (y * multiplier_h) * data_w + (int) (x * multiplier_w);

			if (element > data_w * data_h || element < 0)
				continue;
			
			// RGB
			if (data[element] == -1)
			{
				pixels[element2] = 0;
				pixels[element2 + 1] = 0;
				pixels[element2 + 2] = 255;
			}
			else
			{
				pixels[element2] = 0;
				pixels[element2 + 1] = (data[element] > 255) ? 255 : (unsigned char)data[element];
				pixels[element2 + 2] = 0;
			}
		}
	}

	SaveBMP(("../data/tmp_solution" + to_string(count) + ".bmp").c_str(), image_w, image_h, dpi, pixels);

	delete[] pixels;
}

void Problem::WriteToBMP(char *filename)
{
	data_w = min(instance->GetDimensionX(), (int) sqrt(INT_MAX));
	data_h = min(instance->GetDimensionY(), (int) sqrt(INT_MAX));

	int dpi = 72;
	unsigned char *pixels = new unsigned char[image_w * image_h * 3];
	int element;
	int middle_x = image_w / 2;
	int middle_y = image_h / 2;
	int point_size = 2;

	SetMultiplier();

	int x_distance = (int) (instance->GetMiddleX() * (((float) image_w / (float) data_w) / 1.2));
	int y_distance = (int) (instance->GetMiddleY() * (((float) image_h / (float) data_h) / 1.2));

	multiplier /= 1.2;

	// Create completely white image
	std::fill(pixels, pixels + image_w * image_h * 3, 255);


 	// vector<Point> &points = *instance->GetPoints();

	// cout << points.size() << endl;
	// for (size_t i = 0; i < points.size(); i++)
	// {
	// 	cout << points[i].ToString() << endl;
	// }

	// Print box border and content
	for (auto p = instance->points.begin(); p != instance->points.end(); p++)
	{
		if (!p->HasBox())
			continue;

		int l = p->box.length;
		int h = p->box.height;
		for (int x = (int) (p->box.x * multiplier + middle_x - x_distance); x <= (p->box.x + p->box.length) * multiplier + middle_x - x_distance; x++) {
			for (int y = (int) ((p->box.y - p->box.height) * multiplier + middle_y - y_distance); y <= p->box.y * multiplier + middle_y - y_distance; y++) {

				element = (y * image_w + x) * 3;
				if (element + 3 > image_w * image_h * 3 || element < 0)
					continue;

				if (x == (int) (p->box.x * multiplier + middle_x - x_distance) ||
					y == (int) ((p->box.y - p->box.height) * multiplier + middle_y - y_distance) ||
					x == (int) ((p->box.x + p->box.length) * multiplier + middle_x - x_distance) ||
					y == (int) (p->box.y * multiplier + middle_y - y_distance)
					)
				{
					pixels[element    ] = 0;
					pixels[element + 1] = 0;
					pixels[element + 2] = 0;
				}
				else
				{
					pixels[element    ] = 0;
					pixels[element + 1] = 255;
					pixels[element + 2] = 0;
				}
			}
		}
	}

	// Print the Points
	for (auto p = instance->points.begin(); p != instance->points.end(); p++)
	{
		for (int x = (int) (p->x * multiplier + middle_x - point_size - x_distance); x <= p->x * multiplier + middle_x + point_size - x_distance; x++) {
			for (int y = (int) (p->y * multiplier + middle_y - point_size - y_distance); y <= p->y * multiplier + middle_y + point_size - y_distance; y++) {

				element = (y * image_w + x) * 3;
				if (element + 3 > image_w * image_h * 3 || element < 0)
					continue;

				pixels[element    ] = 255;
				pixels[element + 1] = 0;
				pixels[element + 2] = 0;
			}
		}
	}

	string file = string(filename);
	string::size_type pos = file.find_last_of(".");

	SaveBMP((file.substr(0, pos) + "_solution.bmp").c_str(), image_w, image_h, dpi, pixels);

	delete[] pixels;
}