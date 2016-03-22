#ifndef RW_FILE_H
#define RW_FILE_H

#include <fstream>
#include <string>



using std::fstream;
using std::string;


const int BLOCK_SIZEF = 1024;
const int NODE_SIZEF = 64;
const int FIRST_BLOCKF =  524288;

class RW_FILE{
	fstream rw_file;

public:
	RW_FILE(){

	}
	~RW_FILE(){
		rw_file.clear();
		rw_file.close();
	}
public:
	bool create(const string& name_of_file);
	bool open(const string& name_of_file);
	void flush();
	long long unsigned get_read_position();
	long long unsigned get_write_position();
	void go_to(long long unsigned place);
	void go_to_w(long long unsigned place);
	void go_to_block_w(int block);
	void go_to_block_r(int block);
	long long unsigned get_file_size();
public:
	template<typename T>
	bool write(int node_number, int sector, const T* data, long long unsigned size){
		rw_file.seekp(sector*BLOCK_SIZEF + node_number*NODE_SIZEF);
		rw_file.write((char*)data, size);
		if (!rw_file){
			rw_file.clear();
			return false;
		}
		return true;
	}
	template<typename T>
	bool write(const T* data, long long unsigned size){
		rw_file.write((char*)data, size);
		if (!rw_file){
			rw_file.clear();
			return false;
		}
		return true;
	}
	template<typename T>
	bool write(size_t block, const T* data, long long unsigned size){
		rw_file.seekp(block * BLOCK_SIZEF + FIRST_BLOCKF * BLOCK_SIZEF);
		rw_file.write((char*)data, size);
		if (!rw_file){
			rw_file.clear();
			return false;
		}
		return true;
	}
	template<typename T>
	bool write_from_start(const T* data, long long unsigned size){
		rw_file.seekp(std::ios::beg,0);
		rw_file.write((char*)data, size);
		if (!rw_file){
			rw_file.clear();
			return false;
		}
		return true;
	}
	template<typename T>
	bool write_n_bytes(size_t place, const T* data, long long unsigned size){
		rw_file.seekp(place);
		rw_file.write((char*)data, size);
		if (!rw_file){
			rw_file.clear();
			return false;
		}
		return true;
	}
	template<typename T>
	bool write_after_n_bytes(size_t n, const T* data, long long unsigned size){
		long long unsigned pos = rw_file.tellp();
		pos = pos + n;
		rw_file.seekp(pos);
		rw_file.write((char*)data, size);
		if (!rw_file){
			rw_file.clear();
			return false;
		}
		return true;
	}
	template<typename T>
	bool read_from_start(const T* data, long long unsigned size){
		rw_file.seekg(0);
		rw_file.read((char*)data, size);
		if (!rw_file){
			rw_file.clear();
			return false;
		}
		return true;
	}
	template<typename T>
	bool read(int node_number, int sector, const T* data, long long unsigned size){
		rw_file.seekg(sector * BLOCK_SIZEF + node_number * NODE_SIZEF);
		rw_file.read((char*)data, size);
		if (!rw_file){
			return false;
		}
		return true;
	}
	template<typename T>
	bool read(const T* data, long long unsigned size){
		rw_file.read((char*)data, size);
		if (!rw_file){
			return false;
		}
		return true;
	}
	template<typename T>
	bool read(size_t block, const T* data, long long unsigned size){
		rw_file.seekg(block * BLOCK_SIZEF + BLOCK_SIZEF * FIRST_BLOCKF);
		rw_file.read((char*)data, size);
		if (!rw_file){
			return false;
		}
		return true;
	}
	template<typename T>
	bool read_after_n_bytes(size_t n, const T* data, long long unsigned size){
		rw_file.seekg(n, std::ios::cur);
		rw_file.read((char*)data, size);
		if (!rw_file){
			return false;
		}
		return true;
	}
	template<typename T>
	bool read_from_nth_byte(size_t n, const T* data, long long unsigned size){
		rw_file.seekg(n);
		rw_file.read((char*)data, size);
		if (!rw_file){
			return false;
		}
		return true;
	}
	
};


#endif // !RW_FILE_H
