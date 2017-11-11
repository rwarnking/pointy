#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#define DEBUG false

#include <vector>
#include <iostream>
#include <string>
#include <exception>
#include <fstream>
#include <algorithm>
#include <chrono>

#include "color.h"
#include "bitmap.h"

class Problem
{

public:

	struct Box
	{
		int x, y, length, height;
		std::string label;

		Box() :
			x(0),
			y(0),
			length(0),
			height(0),
			label("") {}

		Box(int w, int h, std::string s) :
			x(0),
			y(0),
			length(w),
			height(h),
			label(s) {}

		Box(int w, int h, int xdim, int ydim, std::string s) :
			x(xdim),
			y(ydim),
			length(w),
			height(h),
			label(s) {}

		Box(const Box &other)
		{
			x = other.x;
			y = other.y;
			length = other.length;
			height = other.height;
			label = other.label;
		}

		std::string ToString()
		{
			std::string s = std::string("x: ");
			s += std::to_string(x) + ", y: " + std::to_string(y) + ", length: " + std::to_string(length) + ", height: " + std::to_string(height) + ", label: " + label;

			return s;
		}
	};

	struct Point
	{
		int x, y;
		bool has_box;
		Box box;

		Point() :
			x(0),
			y(0),
			has_box(false) {}

		Point(int xdim, int ydim, Box b) :
			x(xdim),
			y(ydim),
			has_box(false) {
			box = b;
		}

		Point(const Point &other)
		{
			x = other.x;
			y = other.y;
			has_box = other.has_box;
			box = Box(other.box);
		}

		std::string ToString()
		{
			std::string s = std::to_string(x);
			s += " " + std::to_string(y) + " " + std::to_string(box.length) + " " + std::to_string(box.height) + " " + box.label + " " +
				 std::to_string(has_box) + " " + std::to_string(box.x) + " " + std::to_string(box.y) + "\n";

			return s;
		}
	};

	Problem();
	~Problem();

	void CheckSolution(char *infile);
	void GenerateSolution(char *infile, char *outfile, bool print = false);

private:

	void ReadFile(char *filename);
	size_t ReadPointCount(std::string content);
	void ReadPoints(std::string content, bool solution = true);

	void WriteFile(char *filename);

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

	int point_count;
	std::vector<Point> points, opt;
};

#endif