#include "Bitmap.h"
#include <sstream>
int main (int argc, const char * argv[]) {
	DWORD testSize;
	string fileName;
	Bitmap *bitmap;
	fileName = argv[1];
	
	bitmap = new Bitmap((char *)fileName.c_str(), true);
	
	return 0;
}
