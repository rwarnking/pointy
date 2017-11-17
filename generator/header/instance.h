#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#define DEBUG_MUL 10
#define IMAGE_MUL 25

#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <fstream>
#include <cmath>

enum CORNER : short
{
    BOT_LEFT = 0,
    TOP_LEFT,
    TOP_RIGHT,
    BOT_RIGHT,
    NONE
};

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

class Instance
{
public:

    Instance();
    Instance(const char* filename, bool read_solution=false);
    Instance(std::string filename, bool read_solution=false);
    Instance(std::vector<Point> *p);
    ~Instance();

    int GetPointCount();
    std::vector<Point>* GetPoints();
    int GetRangeX();
    int GetRangeY();

    void SetPointCount(int count);
    void SetPoints(std::vector<Point> *p);

    void WriteFile(const char *filename);

    int point_count;
    std::vector<Point> points;

private:

    void ReadFile(const char *filename, bool read_solution=false);
    size_t ReadPointCount(std::string content);
    void ReadPoints(std::string content, bool read_solution=false);

    // Min/Max point values
    int min_x, max_x;
    int min_y, max_y;
};

#endif // _INSTANCE_H_