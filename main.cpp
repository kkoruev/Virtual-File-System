#include <iostream>
#include <fstream>
#include <cstring>
#include "Dir.h"
#include "File_system.h"
#include "Darray.h"
#include "Info.h"
#include <string>

using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;
using std::ios;
using std::cin;
using std::fstream;




int main(){
	File_system file;
	try{
		file.start();
	}
	catch (std::bad_alloc){
		cout << "Not enough memoty" << endl;
		return -1;
	}
	catch (std::out_of_range){
		cout << "Index out of range" << endl;
		return -1;
	}
	catch (const char* s){
		cout << s << endl;
		return -1;
	}
}