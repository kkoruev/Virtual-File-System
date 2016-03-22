#include "RW_FILE.h"




bool RW_FILE::create(const string& name_of_file){
	rw_file.open(name_of_file, std::ios::out | std::ios::in | std::ios::binary | std::ios::trunc);
	if (!rw_file){
		return false;
	}
	return true;
}

bool RW_FILE::open(const string& name_of_file){
	rw_file.open(name_of_file, std::ios::in | std::ios::out | std::ios::binary);
	if (!rw_file){
		return false;
	}
	return true;
}

void RW_FILE::flush(){
	rw_file.flush();
}


long long unsigned RW_FILE::get_read_position(){
	return rw_file.tellg();
}


void RW_FILE::go_to(long long unsigned place){
	rw_file.seekg(place);
}

void RW_FILE::go_to_w(long long unsigned place){
	rw_file.seekp(place);
}

void RW_FILE::go_to_block_w(int block){
	rw_file.seekp(block * BLOCK_SIZEF + FIRST_BLOCKF * BLOCK_SIZEF);
}

long long unsigned RW_FILE::get_write_position(){
	return rw_file.tellp();
}

void RW_FILE::go_to_block_r(int block){
	rw_file.seekg(block * BLOCK_SIZEF + FIRST_BLOCKF * BLOCK_SIZEF);
}


long long unsigned RW_FILE::get_file_size(){
	rw_file.seekg(std::ios::beg, std::ios::end);
	return rw_file.tellp();
}