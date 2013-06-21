#include <cstdio>
#include <string>
#include <windows.h>
class DataPackage
{
  protected:
    void          *m_Buf;
    unsigned long  m_Size;

  public:
    DataPackage();
    ~DataPackage();

    void *Create(unsigned long Size);
    void Free();

    bool Save(char *Filename);
    void *Load(char *Filename, unsigned long *Size);

    void          *GetPtr();
    unsigned long  GetSize();
}; 