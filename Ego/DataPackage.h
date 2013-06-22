#pragma once

class DataPackage {
	protected:
		void * buffer; ///< buffer for loaded/saved data.
		unsigned long  bufferSize; ///< size of the data buffer.

		DataPackage(unsigned long size, void * buffer);
	public:
		static DataPackage * Load(const char * filename, unsigned long * size);

		DataPackage(unsigned long size);
		~DataPackage();

		bool Save(const char * filename) const;
		
		void *GetPtr() const;
		unsigned long GetSize() const;
};