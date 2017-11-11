#ifndef _COLOR_H
#define _COLOR_H

class Color {
public:
    double r, g, b;

	Color() { r = 0; g = 0; b = 0; };
    ~Color() {};
    Color(double _r, double _g, double _b) : r(_r/255), g(_g/255), b(_b/255) {}

    // omitted operator overloads and print method not used in test case
};

#endif