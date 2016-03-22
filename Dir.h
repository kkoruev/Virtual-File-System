#ifndef DIR_H
#define DIR_H

#include "Darray.h"
#include "File_base.h"
#include "File.h"
#include <iostream>
#include "Nodes.h"
#include "DInfo.h"




using std::string;

class Dir : public File_base {
private:
	Dir* par;
	Dir* sibling;
	Dir* f_child;
	Dir* l_child;
	Darray<File*> files;
	DInfo d_info;
private:
	void copy_from(const Darray<File*> dArr);
	void copy(Dir*& dest, const Dir* src);
	void cpy_files_in_dor(Dir*& ptr, Dir* src);
	Dir(const Dir& dir) : File_base(dir.get_name()), par(dir.par), sibling(dir.sibling), f_child(dir.f_child), l_child(dir.l_child) {
		copy_from(dir.files);
	}
public:
	Dir(string name) : File_base(name), par(NULL), sibling(NULL), f_child(NULL), l_child(NULL) { }

	~Dir(){
		for (int i = 0; i < d_info.cnt_of_files; i++){
			delete files.get_at(i);
		}
		delete f_child;
		delete sibling;
	}
	void add_new_dir(const string& name,int n,int s);
	void add_dir_by_ptr(Dir* ptr,int n,int s);
	void remove_dir_completely(const string& name);
	void change_name(const string& name);
	void copy_from_to(Dir*& dir);
	void copy_from_to_files(File*& file);
	void info();
	void remove_file_by_name(const string& name);
	void chamge_file_name(const string& which_file, const string& name);
	void print_all_children() const;
	Dir* cpy_from(Dir* source);
	Dir* get_par_or_sibling(const string& name,char& c, int& n,int& s);
public:
	void del_p(Dir* p);
	Dir* find_child_by_name(const string& name);
	Dir* prepare_for_move(const string& name);
	Dir* return_parent() const;
	Dir* return_last_child() const;
	Dir* return_f_child() const;
	Dir* rturn_sibling() const;
	DInfo get_info() const;
	void set_info(const DInfo& info);
	void set_first_child_info(int n, int s);
	void set_sibling_info(int n, int s);
	void print_all_files() const;
	size_t get_count_of_files() const;
public:
	void add_file(const string& name,int n,int s,long long unsigned size=0);
	void remove_file(const string& name);
	void change_file_name(const string& which_file, const string& name);
	int get_file_number_by_name(const string& name,char c);
	long long unsigned get_file_size_by_name(const string& name);
	void set_file_size_by_name(const string& name, long long unsigned size);
};

#endif // !DIR_H
