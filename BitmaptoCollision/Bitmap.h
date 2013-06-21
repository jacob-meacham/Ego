#ifndef _BITMAP_H
#define _BITMAP_H

#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include "DataPackage.h"
using namespace std;

#pragma pack(push,bitmap_data,1)

// following structures are the structures used in the windows bmp (.bmp) format.
typedef struct tagRGBQuad {
	char rgbBlue;
	char rgbGreen;
	char rgbRed;
	char rgbReserved;
} RGBQuad;

typedef struct tagBitmapFileHeader {
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} BitmapFileHeader;

typedef struct tagBitmapInfoHeader {
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} BitmapInfoHeader;

#pragma pack(pop,bitmap_data)

class Bitmap {
public:
    
    Bitmap(char *file, bool debug = false);
    ~Bitmap();
    bool loadBMP(char *file);
	void setDebug(bool dbg);

private:
    BitmapFileHeader bmfh;
    BitmapInfoHeader bmih;
	RGBQuad *colors;
	bool debug;

	char *output;
	DWORD testSize;
	DataPackage DP;
    
    unsigned int dataSize;                //size of the data in the file
   
    void reset(void);
	
};

#endif //_BITMAP_H

