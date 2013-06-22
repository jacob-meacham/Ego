#include "DataPackage.h"
#include "Framework\Define.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////
DataPackage::DataPackage(unsigned long size) : bufferSize(size) 
{
	// create a new char array, cast to a void*
	buffer = (void*)new char[bufferSize];
}
//////////////////////////////////////////////////////////////////////////////////
DataPackage::DataPackage(unsigned long size, void * buffer) : bufferSize(size), buffer(buffer)
{

}
//////////////////////////////////////////////////////////////////////////////////
DataPackage::~DataPackage() 
{ 
	delete buffer;
	buffer = NULL;
	bufferSize = 0;
}
//////////////////////////////////////////////////////////////////////////////////
/// Saves the data package to the specified filename.
bool DataPackage::Save(const char *Filename) const {
	// File pointer
	FILE *fp;
	// as long as both the buffer and a size exist, we can write.
	if(buffer != NULL && bufferSize) {
		fopen_s(&fp, Filename, "wb");
		if(fp != NULL) {
			// first write the size of the data.
			fwrite(&bufferSize, 1, 4, fp);
			// then write the data.
			fwrite(buffer, sizeof(char), bufferSize, fp);
			fclose(fp);
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////////
/// Loads a data package from the specified file name.
/** Load can load any data package save by DataPackage.
	\param Filename The name of the file to load
	\param Size A pass-by-reference variable that, when Load
	completes, holds the size of the data package.
	\return a void* to the data package (cast it to the correct type)
*/
DataPackage * DataPackage::Load(const char *Filename, unsigned long *Size) {
	unsigned long loaded_size = 0;

	FILE *fp;
	fopen_s(&fp, Filename, "rb");
	if(fp != NULL) {
		// Read in size and data
		fread(&loaded_size, 1, 4, fp);
		void * loaded_buf;
		if((loaded_buf = (void*)new char[loaded_size]) != NULL) {
			fread(loaded_buf, sizeof(char), loaded_size, fp);
		}
		fclose(fp);

		// Store size to return
		if(Size != NULL) {
			*Size = loaded_size;
		}

		// return pointer
		return new DataPackage(loaded_size, loaded_buf);
	}
	// if the file cannot be opened, return a null package.
	TRACE("DataPackage is NULL!");
	return NULL;
}
//////////////////////////////////////////////////////////////////////////////////
/// Return the void* to the data buffer.
void *DataPackage::GetPtr() const
{
	return buffer;
}
//////////////////////////////////////////////////////////////////////////////////
/// Return the size of the data buffer.
unsigned long DataPackage::GetSize() const
{
	return bufferSize;
}
//////////////////////////////////////////////////////////////////////////////////