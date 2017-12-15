#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "instance.h"

enum ALGORITHM : short
{
	GRAPHIC = 0,
	SIMULATED_ANNEALING,
	IDIOT
};

class Problem
{

public:

	Problem();
	Problem(char *instance_file);
	~Problem();

	size_t Solve(ALGORITHM algorithm);

	void CheckSolution(char *infile);
	void GenerateSolution(char *infile, char *outfile, bool print = false, ALGORITHM algorithm = IDIOT);

private:

	void SimpleSolve();
	CORNER GetBestOrientation(int x, int y);

	int GetBoxCount(ALGORITHM which = IDIOT);
	int IsFeasible();

	bool Intersects(Box p1, Box p2);
	bool Intersects(int one, int two);

	bool Generator(int index = 0);
	bool GeneratorArray(int index = 0);

	void Translate(bool reverse = false);

	bool CheckBox(int* pixels, int px, int py, int ll, int lr, int ho, int hu);
	double CalculateOverlap(int* pixels, int px, int py, int ll, int lr, int ho, int hu);
	void CleanPointArea(Point, int*);

	void SetMultiplier();
	void WriteToBMP(char *filename);
	void WriteToBMP(int count, int *pixels);

	Instance *instance;
	std::vector<Point> opt;

	int data_w, data_h;
	int image_w = 1000, image_h = 1000;
	float multiplier;
	int intensity = 20;
};

#endif