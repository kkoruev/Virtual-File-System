#ifndef DINFO_H
#define DINFO_H

#include <string>
#include <iostream>

#pragma pack(push,1)

using std::string;
using std::cout;
using std::endl;

const char FIRST_CHILD_SIGN_IN_FILE = 'f';
const char SIBLING_SIGN_IN_FILE = 's';
const int INVALID = -8;
const int DIR_S = 1;

struct DInfo{
	int dir;
	char child;
	int c_number;
	int c_sector;
	char sibling;
	int s_number;
	int s_sector;
	size_t cnt_of_files;

public:
	DInfo(){
		dir = DIR_S;
		child = FIRST_CHILD_SIGN_IN_FILE;
		c_number = INVALID;
		c_sector = INVALID;
		sibling = SIBLING_SIGN_IN_FILE;
		s_number = INVALID;
		s_sector = INVALID;
		cnt_of_files = 0;
	}
public:
	void print() const{
		cout << dir << endl;
		cout << child << endl;
		cout << c_number << endl;
		cout << c_sector << endl;
		cout << sibling << endl;
		cout << s_number << endl;
		cout << s_sector << endl;
		cout << cnt_of_files << endl;
	}

	
};

#pragma pack(pop)

#endif // !DINFO_H
