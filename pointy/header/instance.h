#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include <string>
#include <vector>

enum CORNER : short
{
	BOT_LEFT = 0,
	TOP_LEFT,
	TOP_RIGHT,
	BOT_RIGHT,
	NONE
};

class Box
{
public:

	int x, y, length, height;
	CORNER corner;
	std::string label;

	Box();
	Box(int w, int h, std::string s);
	Box(int w, int h, int xdim, int ydim, std::string s);
	Box(const Box &other);

	std::string ToString();
	
	void SetCorner(int px, int py, CORNER new_corner);
	bool Intersects(Box &other);
};

class Point
{
public:

	int x, y;
	Box box;

	Point();
	Point(int xdim, int ydim, Box b);
	Point(const Point &other);

	std::string ToString();

	bool HasBox();
	void ComputeBoxFromCoords();
};

class Instance
{
public:

	Instance();
	Instance(const char* filename, bool read_solution = false);
	Instance(std::string filename, bool read_solution = false);
	Instance(std::vector<Point> *p);
	~Instance();

	size_t GetPointCount();
	std::vector<Point>* GetPoints();

	int MaxX();
	int MinX();
	int MaxY();
	int MinY();

	int GetDimensionX();
	int GetDimensionY();
	int GetMiddleX();
	int GetMiddleY();

	void SetPointCount(int count);
	void SetPoints(std::vector<Point> *p);

	void WriteFile(const char *filename);

	size_t point_count;
	std::vector<Point> points;

private:

	void ReadFile(const char *filename, bool read_solution = false);
	size_t ReadPointCount(std::string content);
	void ReadPoints(std::string content, bool read_solution = false);

	size_t FindSpaceOrTab(std::string content, size_t start=0);
	size_t SkipSpacesOrTabs(std::string content, size_t start=0);

	// Min/Max point values
	int min_x, max_x;
	int min_y, max_y;
	// Min/Max edge length
	int min_l, max_l;
	int min_h, max_h;
};

#endif // _INSTANCE_H_