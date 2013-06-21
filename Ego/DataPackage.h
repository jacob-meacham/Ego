#pragma once

class DataPackage {
	protected:
		void          *m_Buf; ///< buffer for loaded/saved data.
		unsigned long  m_Size; ///< size of the data buffer.
		DataPackage(unsigned long size, void * buffer);
	public:
		DataPackage(unsigned long size);
		~DataPackage();

		void *Create(unsigned long Size);
		void Free();

		bool Save(const char *Filename);
		static DataPackage * Load(const char *Filename, unsigned long *Size);

		void *GetPtr();
		unsigned long GetSize() const;
};