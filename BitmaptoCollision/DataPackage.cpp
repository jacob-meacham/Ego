#include "DataPackage.h"
DataPackage::DataPackage() { if(m_Buf) m_Buf = NULL; m_Size = 0; }

DataPackage::~DataPackage() { Free(); }

void *DataPackage::Create(unsigned long Size) {
	Free();
	m_Size = Size;
	m_Buf = (void*)new char[m_Size];
	return m_Buf;
}

void DataPackage::Free() {
	delete m_Buf;
	m_Buf = NULL;
	m_Size = 0;
}

bool DataPackage::Save(char *Filename) {
	FILE *fp;
	if(m_Buf != NULL && m_Size) {
		fopen_s(&fp, Filename, "wb");
		if(fp != NULL) {
			fwrite(&m_Size, 1, 4, fp);
			fwrite(m_Buf, sizeof(char), m_Size, fp);
			fclose(fp);
			return true;
		}
	}
	return false;
}

void *DataPackage::Load(char *Filename, unsigned long *Size)
{
  FILE *fp;

  // Free a prior buffer
  Free();

  fopen_s(&fp, Filename, "rb");
  if(fp != NULL) {
    // Read in size and data
    fread(&m_Size, 1, 4, fp);
    if((m_Buf = (void*)new char[m_Size]) != NULL)
      fread(m_Buf, sizeof(char), m_Size, fp);
    fclose(fp);

    // Store size to return
    if(Size != NULL)
      *Size = m_Size;

    // return pointer
    return m_Buf;
  }
  return NULL;
}

void *DataPackage::GetPtr()
{
  return m_Buf;
}

unsigned long DataPackage::GetSize()
{
  return m_Size;
}
