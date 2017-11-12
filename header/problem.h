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

	int GetBoxCount(short which = 0);
	int IsFeasible();

	bool Intersects(Box p1, Box p2);
	bool Intersects(int one, int two);

	bool Generator(int index = 0);
	bool GeneratorArray(int index = 0);
	
	bool CheckBox(Color* pixels, int px, int py, int ll, int lr, int ho, int hu, int intensity);
	float CalculateOverlap(Color* pixels, int px, int py, int ll, int lr, int ho, int hu, int intensity);
	void Clean(Color* pixels);

	void WriteToConsole();
    void WriteToBMP(char *filename);

    Instance *instance;
    std::vector<Point> opt;
};

#endif