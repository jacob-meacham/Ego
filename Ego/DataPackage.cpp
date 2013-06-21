#include "DataPackage.h"
#include "Framework\Define.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////
DataPackage::DataPackage(unsigned long size) : m_Size(size) 
{
	// create a new char array, cast to a void*
	m_Buf = (void*)new char[m_Size];
}
//////////////////////////////////////////////////////////////////////////////////
DataPackage::DataPackage(unsigned long size, void * buffer) : m_Size(size), m_Buf(buffer)
{

}
//////////////////////////////////////////////////////////////////////////////////
DataPackage::~DataPackage() 
{ 
	delete m_Buf;
	m_Buf = NULL;
	m_Size = 0;
}
//////////////////////////////////////////////////////////////////////////////////
/// Saves the data package to the specified filename.
bool DataPackage::Save(const char *Filename) {
	// File pointer
	FILE *fp;
	// as long as both the buffer and a size exist, we can write.
	if(m_Buf != NULL && m_Size) {
		fopen_s(&fp, Filename, "wb");
		if(fp != NULL) {
			// first write the size of the data.
			fwrite(&m_Size, 1, 4, fp);
			// then write the data.
			fwrite(m_Buf, sizeof(char), m_Size, fp);
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
void *DataPackage::GetPtr()
{
	return m_Buf;
}
//////////////////////////////////////////////////////////////////////////////////
/// Return the size of the data buffer.
unsigned long DataPackage::GetSize() const
{
	return m_Size;
}
//////////////////////////////////////////////////////////////////////////////////