#ifndef FILE_H
#define FILE_H

#include "File_base.h"


const string EXT = ".f";

class File : public File_base{
private:
	long long unsigned size;

public:
	long long unsigned get_size() const;

public:
	File(string name) : File_base(name), size(0) {};
	string toString() const;
	void set_size(long long unsigned size);
};


#endif // !FILE_H
