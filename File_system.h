#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "Dir.h"
#include <string>
#include "Nodes.h"
#include <fstream>
#include "RW_FILE.h"
#include "Info.h"
#include "DInfo.h"
#include <sys\stat.h>
#include "include\dirent.h"


using std::string;


const int sign = -1;
const size_t max_inodes = 8388608;
const size_t first_block = 524288;
const size_t count_of_used_blocks = 0;
const size_t first_block_to_use = 0;
const size_t first_node = 0;
const int next = -2;


const int SIGN = 0;
const int COUNT_OF_NODES = 1;
const int MAX_NODES = 2;
const int COUNT_OF_USED_BLOCKS = 3;
const int FIRST_BLOCK = 4;
const int NODE_NUM = 5; //(root dir)
const int NODE_SECTOR = 6; //(root dir)
const int FIRST_BLOCK_TO_USE = 7;
const int NEXT = 8;


const int COUNT_OF_FIRST_EL = 9;
const int BLOCK_SIZE = 1024;
const int NODE_SIZE = 64;
const size_t MAX_LEN_NAME_DIR  = 120;
const int NODES_PLACE_SIBLING = sizeof(int)+120 + sizeof(char)+sizeof(int)+sizeof(int);
const int NODES_PLACE_CHILD = sizeof(int)+120;


const int END_SIGN = -18;

const int FIRST_UN_N = 100;
const int FIRST_UN_B = 600;

const int DIR = 1;
const int FILES = 0;

const long long unsigned EMPTY_FILE = 0;


const int COUNT_OF_BLOCKS_IN_NODE = 16;
const int COUNT_OF_MAIN_BLOCKS_IN_NODE = 14;

const int MAX_BYTES_IN_FILE_BLOCK = BLOCK_SIZE - (sizeof(int)+sizeof(EMPTY_FILE));
const int MAX_BYTES_IN_FILES_BLOCK_WITH_NAME = MAX_BYTES_IN_FILE_BLOCK - MAX_LEN_NAME_DIR;

const int MAX_COUNT_OF_FILES_IN_BEG_DIR = (BLOCK_SIZE - (sizeof(DInfo) + MAX_LEN_NAME_DIR)) / (2 * sizeof(int));



const int N = 10;
const int M = 100000;
const int ARR_SIZE_FOR_EXPORT = BLOCK_SIZE * N;
const int ARR_SIZE_FOR_IMPORT = BLOCK_SIZE * M ;




class File_system{

private:
	Dir* root;
	Dir* cur;
	RW_FILE file;
	Info information;
	string name_of_file_system;
	int first_unused_node;
	int firsT_unused_block;
private:
	File_system(const File_system& system);
	File_system& operator=(const File_system& system);

public:
	File_system() : root(NULL), first_unused_node(-1) , firsT_unused_block(-1) { init_info();  }
	~File_system(){
		delete root;
	}

private:
	// system methods for print and etc.
	int print_start_text();

private:
	// load and start methods
	bool load_file(const string& name_of_file);
	bool start_file_system(const string& name_of_file,const string& name_of_main_dir);

private:
	 // methods for blocks and index nodes
	int find_main_block(int n, int s);
	bool init_number_and_sector(int& n, int& s);
	int make_inode(int n, int s);
	int get_free_block();
	long long unsigned get_next_free_node_place(int n, int s);
	bool make_block(long long unsigned place);
private:
	// dir functions
	Dir* find_dir_by_path(const string& path);
	bool save_dir_in_file(int place, const string& name);
	bool save_dir_in_file_helper(Dir* ptr, const string& name,char c);
	Dir* load_dir_from_file(int n, int s);
	bool change_dir_name_in_file(int n, int s, const string& name);
	bool change_dir_after_remove(Dir* p, char sign, int n, int s);
	bool remove_file_from_dir(int dir_n, int dir_s, int f_n, int f_s);
	bool add_new_dir_file(const string& name);
	void invalid_dir(Dir* ptr);
	bool save_dir_in_file_after_move(Dir* ptr, int n, int s, DInfo info, char c);
public:
	//public methods
	void start();
private:
	// command methods
	bool execute_command(const Darray<string>& tokens);
	bool execute_single_command(const Darray<string>& tokens);
	bool create_dir(const string& name);
	bool remove_dir(const string& name);
	bool change_dir_name(const string& which_dir, const string& name);
	bool move_dir(const string& which_dir, const string& path);
	bool copy_dir(const string& which_dir, const string& path);
	bool dir_info();
	bool create_file(const string& name);
	bool remove_file(const string& name);
	bool change_file_name(const string& which_file, const string& name);
	bool move_file(const string& which_file, const string& path);
	bool copy_file(const string& which_file, const string& path);
	bool info_file(const string& name_of_file);
	bool apend_text(const string& name_of_file, const string& text);
	void init_info();
	bool export_file(const string& name, const string& path);
	bool import_file(const Darray<string> tokens);
private:
	// other methods
	bool check_tokens_for_append(string& text, const Darray<string>& tokens);

public:

private:
	// methods for files	
	bool save_file_in_file_system(const string& name, int n, int s);
	bool save_file_in_dir_in_file_system(int dir_n, int dir_s, int f_n, int f_s);
	bool load_file_from_file_system(Dir* ptr, int n, int s);
	int next_file_pos(int n) const;
	bool change_file_name_in_file_system(const string& name, int n, int s);
	bool swap_last_with_cur_in_file(int cnt, int cnt_of_files, int place);
	bool save_file_after_remove_in_new_dir(Dir* p, int n, int s);
	bool save_text(int n, int s, long long unsigned  size, const string& text);
	bool save_text_in_file(int block, long long unsigned size, const string& text,int cnt);
	bool save_file_size(int n,int s,long long unsigned new_size);
	bool save_text_helper(const string& text, long long unsigned size, int block, long long unsigned place,int cnt_of_node);
	long long unsigned get_file_size(int block);
	bool export_helper(RW_FILE& file,char* arr,int node_cnt,int arr_size,int arr_capacity,long long unsigned place);
	bool write_array_in_file(RW_FILE& export_file, char* arr, int arr_size);
	bool fill_array(char* arr, int& arr_size, int block_size, int block, int cnt_node);
	bool import_helper(RW_FILE& reader, const string& name);
	bool read_and_write_after_import(long long unsigned size, RW_FILE& reader, long long unsigned place, int cnt_of_node);
	bool divide_bytes(char* arr, long long unsigned size, long long unsigned& place, int& cnt_of_node);
	bool write_bytes_to_block_after_import(char* arr, int bytes_to_append, int block, int skip);
	bool init_secondary_block_in_file(int block);
	bool func(char * arr, int block, long long unsigned& size, long long unsigned& place, int& cnt_of_node);
	bool print_file_info(int n, int s);
private:
	// test functions
	void test_append(int n, int s);
	string text_for_append();
	void test_xport(int n, int s);
	
};
#endif // !FILE_SYSTEM
