#ifndef FILE_BASE_H
#define FILE_BASE_H

#include <cstring>
#include <string>

using std::string;

class File_base{
	string name;
	size_t number;
	size_t sector;
public:
	File_base(string name_c) : name(name_c),number(0),sector(0) {}
	string get_name() const;
	void set_name(const string& name);
	int get_cnt() const{
		return number;
	}
	int get_sector() const{
		return sector;
	}
	void set_number(size_t number,size_t sector){
		this->number = number;
		this->sector = sector;
	}
};

#endif // !FILE_BASE_H
