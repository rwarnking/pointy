#include "../header/problem.h"

using namespace std;

Problem::Problem()
{
	instance = new Instance();
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
	case 0: cout << GetBoxCount() << endl; break;
	default: cout << "ERROR: unknown problem" << endl;
	}
}

// Read problem in 'infile', then generate solution(s) and
// write the solution to 'outfile'
void Problem::GenerateSolution(char *infile, char *outfile, bool print, short algorithm)
{
	instance = new Instance(infile);

	// Take time
	chrono::steady_clock::time_point start = chrono::steady_clock::now();

	// Sort from smallest to biggest box
	sort(instance->points.begin(), instance->points.end(), [](Point &p1, Point &p2) { return p1.box.length * p1.box.height < p2.box.length * p2.box.height; });

	//Generator();
	//instance->points = vector<Point>(opt);
	switch (algorithm)
	{
		case 0: SimpleSolve(); break;
		case 1: GeneratorArray(); break;
		case 2: Generator(); break;
		default: cout << "Invalid algorithm parameter" << endl;
	}

	// Take time
	chrono::steady_clock::time_point end = chrono::steady_clock::now();

	//WriteToConsole();
	cout << GetBoxCount(algorithm == 0 ? 0 : 1) << "\t" << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;

	if (print)
		WriteToBMP(outfile);

	instance->WriteFile(outfile);
}

void Problem::SimpleSolve()
{
	vector<Point>& points = *instance->GetPoints();

	for (size_t i = 0; i < points.size(); i++)
	{
		CORNER c = GetBestOrientation(points[i].x, points[i].y);
		points[i].has_box = true;
		switch (c)
		{
			// Lower left corner
			case BOT_LEFT: points[i].box.x = points[i].x; points[i].box.y = points[i].y + points[i].box.height; break;
			// Upper left corner
			case TOP_LEFT: points[i].box.x = points[i].x; points[i].box.y = points[i].y; break;
			// Upper right corner
			case TOP_RIGHT: points[i].box.x = points[i].x - points[i].box.length; points[i].box.y = points[i].y; break;
			// Lower right corner
			case BOT_RIGHT: points[i].box.x = points[i].x - points[i].box.length; points[i].box.y = points[i].y + points[i].box.height; break;
			default: cout << "error" << endl; break;
		}
		for (size_t j = 0; j < i; j++)
		{
			if (Intersects(points[i].box, points[j].box))
			{
				points[i].has_box = false;
				break;
			}
		}
	}
}

CORNER Problem::GetBestOrientation(int x, int y)
{
	int middle_x = instance->GetMiddleX();
	int middle_y = instance->GetMiddleY();

	if (x < middle_x)
		return y < middle_y ? TOP_RIGHT : BOT_RIGHT;
	else
		return y < middle_y ? TOP_LEFT : BOT_LEFT;
}

bool Problem::Generator(int index)
{
	int point_count = instance->point_count;

	int tmp1 = GetBoxCount();
	int tmp2 = GetBoxCount(1);

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
		case NONE: p->has_box = false; return Generator(index + 1);
		default: cout << "error" << endl; break;
		}

		bool intersects = false;
		for (auto it2 = instance->points.begin(); it2 != instance->points.end() && !intersects; it2++)
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

	// Loop over all instance->points and select the box that has no overlaps
	for (auto p = instance->points.begin(); p != instance->points.end(); p++)
	{
		// Skip element if it already has a box
		if (p->has_box)
			continue;

		for (short corner = 0; corner < 4; corner++)
		{
			switch (corner)
			{
				// Lower left corner
			case BOT_LEFT:
			{
				if (CheckBox(pixels, p->x, p->y, 0, p->box.length, p->box.height, 0)) {

					CleanPointArea(*p, pixels);

					p->has_box = true;
					p->box.x = p->x;
					p->box.y = p->y + p->box.height;
					corner = 5;
					p = instance->points.begin();
				}
				break;
			}
			// Upper left corner
			case TOP_LEFT:
			{
				if (CheckBox(pixels, p->x, p->y, 0, p->box.length, 0, p->box.height)) {

					CleanPointArea(*p, pixels);

					p->has_box = true;
					p->box.x = p->x;
					p->box.y = p->y;
					corner = 5;
					p = instance->points.begin();
				}
				break;
			}
			// Upper right corner
			case TOP_RIGHT:
			{
				if (CheckBox(pixels, p->x, p->y, p->box.length, 0, 0, p->box.height)) {

					CleanPointArea(*p, pixels);

					p->has_box = true;
					p->box.x = p->x - p->box.length;
					p->box.y = p->y;
					corner = 5;
					p = instance->points.begin();
				}
				break;
			}
			// Lower left corner
			case BOT_RIGHT:
			{
				if (CheckBox(pixels, p->x, p->y, p->box.length, 0, p->box.height, 0)) {

					CleanPointArea(*p, pixels);

					p->has_box = true;
					p->box.x = p->x - p->box.length;
					p->box.y = p->y + p->box.height;
					corner = 5;
					p = instance->points.begin();
				}
				break;
			}
			default: cout << "ERROR: switch case 1 " << endl; break;
			}
		}
	}

	int count = 0;
	// Loop over all instance->points and select for each point the box with the least overlaps
	for (auto p = instance->points.begin(); p != instance->points.end(); p++, count++)
	{
		// Skip if element already has a box
		if (p->has_box)
			continue;

		double min;
		double tmp = 50000.0;
		short corner_s = 0;

		for (short corner = 0; corner < 4; corner++)
		{
			switch (corner)
			{
				// Ecke links unten
			case BOT_LEFT:
			{
				min = CalculateOverlap(pixels, p->x, p->y, 0, p->box.length, p->box.height, 0);
				break;
			}
			// Ecke links oben
			case TOP_LEFT:
			{
				tmp = CalculateOverlap(pixels, p->x, p->y, 0, p->box.length, 0, p->box.height);
				if ((min > tmp || min == -1) && tmp != -1)
				{
					min = tmp;
					corner_s = 1;
				}
				break;
			}
			// Ecke rechts oben
			case TOP_RIGHT:
			{
				tmp = CalculateOverlap(pixels, p->x, p->y, p->box.length, 0, 0, p->box.height);
				if ((min > tmp || min == -1) && tmp != -1)
				{
					min = tmp;
					corner_s = 2;
				}
				break;
			}
			// Ecke rechts unten
			case BOT_RIGHT:
			{
				tmp = CalculateOverlap(pixels, p->x, p->y, p->box.length, 0, p->box.height, 0);
				if ((min > tmp || min == -1) && tmp != -1)
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
		if (min != -1)
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
			WriteToBMP(count, pixels);

		int l = p->box.length;
		int h = p->box.height;
		for (int x = (p->x - l) + middle_x; x < (p->x + l) + middle_x; x++) {
			for (int y = (p->y - h) + middle_y; y < (p->y + h) + middle_y; y++) {

				element = y * data_w + x;
				if (pixels[element] != -1)
					pixels[element] = pixels[element] - intensity;
			}
		}

		if (min != -1)
		{
			for (int x = p->box.x + middle_x; x < (p->box.x + p->box.length) + middle_x; x++) {
				for (int y = (p->box.y - p->box.height) + middle_y; y < p->box.y + middle_y; y++) {

					element = y * data_w + x;
					pixels[element] = -1;
				}
			}
		}
	}

	opt = vector<Point>(instance->points);

	delete[] pixels;

	return false;
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

	// THEORETISCH soll hier der flächeninhalt errechnet werden
	// soll heißen
	// gesamtfärbung / flächeninhalt  = durchschnittliche Faerbung

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

void Problem::Clean(int* pixels)
{
	int middle_x = data_w / 2;
	int middle_y = data_h / 2;
	int element;

	for (auto p = instance->points.begin(); p != instance->points.end(); p++)
	{
		if (!p->has_box)
			continue;
		/*
		int l = p->box.length;
		int h = p->box.height;
		for (int x = (p->x - l) + middle_x; x < (p->x + l) + middle_x; x++) {
			for (int y = (p->y - h) + middle_y; y < (p->y + h) + middle_y; y++) {

				element = y * data_w + x;
				if (pixels[element] != -1)
					pixels[element] = pixels[element] - intensity;
			}
		}*/

		CleanPointArea(*p, pixels);
	}
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
	int prev = 1;
	for (auto it = instance->points.begin(); it != instance->points.end(); it++, prev++)
	{
		if (it->has_box)
		{
			for (auto it2 = instance->points.begin() + prev; it2 != instance->points.end(); it2++)
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
	vector<Point> work = which == 0 ? instance->points : opt;
	for (auto it = work.begin(); it != work.end(); it++)
	{
		if (it->has_box)
			count++;
	}

	return count;
}

void Problem::WriteToBMP(int count, int *data)
{
	int dpi = 72;
	int multiplier = (image_w / data_w) * 2;
	float screenaspect = 2.0f / ((float) image_w / (float) data_w);
	Color *pixels = new Color[image_w * image_h];
	int middle_x = data_w / 2;
	int middle_y = data_h / 2;

	int element;
	int element2;

	// Loop over all Pixel around the Center in the Area.
	for (int y = middle_y - data_h / (multiplier * screenaspect); y < middle_y + data_h / (multiplier * screenaspect); y++) {
		for (int x = middle_x - data_w / (multiplier * screenaspect); x < middle_x + data_w / (multiplier * screenaspect); x++) {
			// Element position in pixels array
			element = y * data_w + x;

			for (int i = 0; i < multiplier; i++)
			{
				for (int j = 0; j < multiplier; j++)
				{
					element2 = (y - (middle_y - data_h / (multiplier * screenaspect))) * multiplier * image_w + (x - (middle_x - data_w / (multiplier * screenaspect))) * multiplier + j + i * image_w;

					if (data[element] == -1)
						pixels[element2].b = 1.0;
					else
						pixels[element2].g = data[element] / 255.0;
				}
			}
		}
	}

	savebmp(("../data/tmp_solution" + std::to_string(count) + ".bmp").c_str(), image_w, image_h, dpi, pixels);

	delete[] pixels;
}

void Problem::WriteToBMP(char *filename)
{
	int dpi = 72;
	int width = image_w;
	int height = image_h;
	int n = width * height;
	Color *pixels = new Color[n];
	int element;
	int r, g, b;
	int middle_x = width / 2;
	int middle_y = height / 2;
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
	for (auto p = instance->points.begin(); p != instance->points.end(); p++)
	{
		if (!p->has_box)
			continue;

		int l = p->box.length;
		int h = p->box.height;
		for (int x = p->box.x * multiplier + middle_x; x <= (p->box.x + p->box.length) * multiplier + middle_x; x++) {
			for (int y = (p->box.y - p->box.height) * multiplier + middle_y; y <= p->box.y * multiplier + middle_y; y++) {

				element = y * width + x;
				if (element > n)
					continue;

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

	// Print box instance->points
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