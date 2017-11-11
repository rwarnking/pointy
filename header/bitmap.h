#ifndef _BITMAP_H
#define _BITMAP_H

#include <fstream>
#include <cmath>

#include "color.h"

static void savebmp(const char* filename, int width, int height, int dpi, Color* data) {
    int k = width * height;
    int s = 4 * k;
    int filesize = 54 + s;

    int m = static_cast<int>(39.375);
    int ppm = dpi * m; // pixels per meter

    unsigned char bmpfileheader[14] = {
        'B', 'M', 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0
    };

    unsigned char bmpinfoheader[40] = {
        40, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 24, 0
    };

    // Unsure about this implementation - read more about bitmap creading
    bmpfileheader[2] = (unsigned char)(filesize);
    bmpfileheader[3] = (unsigned char)(filesize >> 8);
    bmpfileheader[4] = (unsigned char)(filesize >> 16);
    bmpfileheader[5] = (unsigned char)(filesize >> 24);

    bmpinfoheader[4] = (unsigned char)(width);
    bmpinfoheader[5] = (unsigned char)(width >> 8);
    bmpinfoheader[6] = (unsigned char)(width >> 16);
    bmpinfoheader[7] = (unsigned char)(width >> 24);

    bmpinfoheader[8] = (unsigned char)(height);
    bmpinfoheader[9] = (unsigned char)(height >> 8);
    bmpinfoheader[10] = (unsigned char)(height >> 16);
    bmpinfoheader[11] = (unsigned char)(height >> 24);

    bmpinfoheader[21] = (unsigned char)(s);
    bmpinfoheader[22] = (unsigned char)(s >> 8);
    bmpinfoheader[23] = (unsigned char)(s >> 16);
    bmpinfoheader[24] = (unsigned char)(s >> 24);

    bmpinfoheader[25] = (unsigned char)(ppm);
    bmpinfoheader[26] = (unsigned char)(ppm >> 8);
    bmpinfoheader[27] = (unsigned char)(ppm >> 16);
    bmpinfoheader[28] = (unsigned char)(ppm >> 24);

    bmpinfoheader[29] = (unsigned char)(ppm);
    bmpinfoheader[30] = (unsigned char)(ppm >> 8);
    bmpinfoheader[31] = (unsigned char)(ppm >> 16);
    bmpinfoheader[32] = (unsigned char)(ppm >> 24);

    std::ofstream file_writer(filename, std::ios::out | std::ios::binary);
    if (file_writer)
    {
        file_writer.write(reinterpret_cast<char*>(&bmpfileheader), sizeof(bmpfileheader));
        file_writer.write(reinterpret_cast<char*>(&bmpinfoheader), sizeof(bmpinfoheader));
        for (int i = 0; i < k; i++) {
            Color rgb = data[i];

            double red = data[i].r * 255;
            double green = data[i].g * 255;
            double blue = data[i].b * 255;

            unsigned char color[3] = { (unsigned char)std::floor(blue),
                                       (unsigned char)std::floor(green),
                                       (unsigned char)std::floor(red) };

            file_writer.write(reinterpret_cast<char*>(&color), sizeof(color));
        }
        file_writer.close();
    }
    else
    {
        //cerr << "Error opening writing stream" << endl;
    }
}

#endif