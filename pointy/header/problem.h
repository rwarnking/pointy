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
	void GenerateSolution(char *infile, char *outfile, bool print=false, short algorithm=0);

private:

	void SimpleSolve();
	CORNER GetBestOrientation(int x, int y);

	int GetBoxCount(short which = 0);
	int IsFeasible();

	bool Intersects(Box p1, Box p2);
	bool Intersects(int one, int two);

	bool Generator(int index = 0);
	bool GeneratorArray(int index = 0);

	bool CheckBox(int* pixels, int px, int py, int ll, int lr, int ho, int hu);
	double CalculateOverlap(int* pixels, int px, int py, int ll, int lr, int ho, int hu);
	void Clean(int* pixels);
	void CleanPointArea(Point, int*);

	void WriteToBMP(char *filename);
	void WriteToBMP(int count, int *pixels);

	Instance *instance;
	std::vector<Point> opt;

	int data_w = 100, data_h = 100;
	int image_w = 500, image_h = 500;
	int multiplier = 25; // (image_w / data_w) * 10;
	int intensity = 20;
};

#endif