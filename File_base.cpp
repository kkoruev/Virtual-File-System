#include "File_base.h"

string File_base::get_name() const{
	return name;
}


void File_base::set_name(const string& name){
	this->name = name;
}