#include "File.h"

string File::toString() const{
	string full_name = get_name();
	full_name.append(EXT);
	return full_name;
}

void File::set_size(long long unsigned size){
	if (size < 0){
		this->size = 0;
	}
	else{
		this->size = size;
	}
}


long long unsigned File::get_size() const{
	return size;
}