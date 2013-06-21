#include "Bitmap.h"

//constructor loads the bitmap when it is created
Bitmap::Bitmap(char *file, bool debug){
    reset();
	setDebug(debug);
    loadBMP(file);
}

//destructor
Bitmap::~Bitmap(){
    if(colors != 0) {
        delete[] colors;
    }
	if(output != 0) {
	delete output;
	}
}

//load a bitmap from a file and represent it correctly
//in memory
bool Bitmap::loadBMP(char *file) {

    FILE *in; //file stream for reading
 
	//open the file for reading in binary mode
    fopen_s(&in, file,"rb");

    //if the file does not exist return in error
    if(in==NULL) {
        cout << "file not found" << endl;
        fclose(in);
        return false;
    }

    //read in the entire BITMAPFILEHEADER
    fread(&bmfh,sizeof(BitmapFileHeader),1,in);

    //check for the magic number that says this is a bitmap
    if(bmfh.bfType!=19778) {
        cout << "File is not a bmp" << endl;
        fclose(in);
        return false;
    }

    //read in the entire BITMAPINFOHEADER
    fread(&bmih,sizeof(BitmapInfoHeader),1,in);
	
	if(debug) {
	cout << "sizeof(BitmapInfoHeader)=" << sizeof(BitmapInfoHeader) << endl;

    //save the width, height and bits per pixel for external use
	cout << "bfSize			 =" << bmfh.bfSize << endl;
	cout << "bfOffBits		 =" << bmfh.bfOffBits << endl;
	cout << "biBitCount      =" << bmih.biBitCount << endl;
	cout << "biClrImportant  =" << bmih.biClrImportant << endl;
	cout << "biClrUsed       =" << bmih.biClrUsed << endl;
	cout << "biCompression   =" << bmih.biCompression << endl;
	cout << "biHeight        =" << bmih.biHeight << endl;
	cout << "biPlanes        =" << bmih.biPlanes << endl;
	cout << "biSize          =" << bmih.biSize << endl;
	cout << "biSizeImage     =" << bmih.biSizeImage << endl;
	cout << "biWidth         =" << bmih.biWidth << endl;
	cout << "biXPelsPerMeter =" << bmih.biXPelsPerMeter << endl;
	cout << "biYPelsPerMeter =" << bmih.biYPelsPerMeter << endl;
	}

    //Calculate data size
	dataSize = bmih.biSizeImage;
	
	// Create Data Package
	output = (char *)DP.Create(dataSize);
	

	// Read in color key
	int numColors=1 << bmih.biBitCount;
	colors=new RGBQuad[numColors];
    fread(colors,sizeof(RGBQuad),numColors,in);

	//read in the entire image
	fread(output, sizeof(char), dataSize, in);
	fclose(in);

	if(debug) {
		ofstream out("output.bmp", ios::binary);
		out.write((char*)(&bmfh), sizeof(BitmapFileHeader));
		out.write((char*)(&bmih), sizeof(BitmapInfoHeader));
		out.write((char*)(&colors), numColors*sizeof(RGBQuad));
		out.write(output, dataSize);
		out.close();
	}
	
	DP.Save("output.col");
	DP.Free();

	//return success	
    return true;
}

//function to set the inital values
void Bitmap::reset(void) {
	debug = false;
    colors=0;
}

void Bitmap::setDebug(bool dbg) {
	debug = dbg;
}

