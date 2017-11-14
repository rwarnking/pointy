#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#define DEBUG false

#include <algorithm>
#include <chrono>

#include "instance.h"
#include "color.h"
#include "bitmap.h"

class Problem
{

public:

	Problem();
	~Problem();

	void CheckSolution(char *infile);
	void GenerateSolution(char *infile, char *outfile, bool print = false);

private:

	void SimpleSolve();
	CORNER GetBestOrientation(int x, int y);

	int GetBoxCount(short which = 0);
	int IsFeasible();

	bool Intersects(Box p1, Box p2);
	bool Intersects(int one, int two);

	bool Generator(int index = 0);
	bool GeneratorArray(int index = 0);

	bool CheckBox(Color* pixels, int px, int py, int ll, int lr, int ho, int hu, int intensity);
	double CalculateOverlap(Color* pixels, int px, int py, int ll, int lr, int ho, int hu, int intensity);
	void Clean(Color* pixels);

	void WriteToBMP(char *filename);
	void WriteToBMP(int count, Color *pixels);

	Instance *instance;
	std::vector<Point> opt;

	int data_w = 30000, data_h = 30000;
	int image_w = 30000, image_h = 30000;
};

#endif