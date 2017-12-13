#ifndef _BITMAP_H
#define _BITMAP_H

//#define WINVER 0x0501
//#define _WIN32_WINNT 0x0501

#include <fstream>
#include <cmath>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/sysinfo.h>
#endif

static size_t GetRAM()
{
#ifdef _WIN32
    typedef BOOL(WINAPI *PGMSE)(LPMEMORYSTATUSEX);
    PGMSE pGMSE = (PGMSE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), (LPCSTR) TEXT("GlobalMemoryStatusEx"));
    if (pGMSE != 0)
    {
        MEMORYSTATUSEX mi;
        memset(&mi, 0, sizeof(MEMORYSTATUSEX));
        mi.dwLength = sizeof(MEMORYSTATUSEX);
        if (pGMSE(&mi) == TRUE)
            return mi.ullTotalPhys;
        else
            pGMSE = 0;
    }
    if (pGMSE == 0)
    {
        MEMORYSTATUS mi;
        memset(&mi, 0, sizeof(MEMORYSTATUS));
        mi.dwLength = sizeof(MEMORYSTATUS);
        GlobalMemoryStatus(&mi);
        return mi.dwTotalPhys;
    }
    return 0;
#else
    struct sysinfo info;

    int sysinfo(&info);

    return info.totalram;
#endif
}

static void SaveBMP(const char* filename, int width, int height, int dpi, unsigned char* data) 
{
    FILE *f;
    int k = width * height * 3;
    int s = 4 * k;
    int filesize = 54 + s;

    int m = static_cast<int>(39.375);
    int ppm = dpi * m; // pixels per meter

    unsigned char bmpfileheader[14] = 
    {
        'B', 'M', 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0
    };

    unsigned char bmpinfoheader[40] = 
    {
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

    fopen_s(&f, filename, "wb");
    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);

    for (int i = 0; i < k; i += 3)
    {
        unsigned char color[3] = { data[i + 2], data[i + 1], data[i] };

        fwrite(color, 1, 3, f);
    }
    fclose(f);
}

#endif