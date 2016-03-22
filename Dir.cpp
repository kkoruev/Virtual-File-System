#include "Dir.h"
#include <iostream>

using std::cout;
using std::endl;


void Dir::add_new_dir(const string& name,int n,int s){
	Dir* temp = new Dir(name);
	temp->set_number(n,s);
	if (l_child == NULL){
		f_child = temp;
		l_child = temp;
		f_child->par = this;
	}
	else{
		l_child->sibling = temp;
		l_child = temp;
		l_child->par = this;
	}
}

void Dir::add_dir_by_ptr(Dir* ptr, int n,int s){
	ptr->set_number(n,s);
	if (l_child == NULL){
		f_child = ptr;
		l_child = ptr;
		f_child->par = this;
	}
	else{
		l_child->sibling = ptr;
		l_child = ptr;
		l_child->par = this;
	}
}

void Dir::remove_dir_completely(const string& name){

}

void Dir::change_name(const string& name){
	set_name(name);
}

void Dir::copy_from_to(Dir*& dir){

}

void Dir::info(){

}

Dir* Dir::find_child_by_name(const string& name){
	Dir* n = f_child;
	if (n){
		while (n){
			if (n->get_name() == name){
				return n;
			}
			n = n->sibling;
		}
	}
	else{
		return NULL;
	}
	return NULL;
}

Dir* Dir::prepare_for_move(const string& name){
	Dir* prev = NULL;
	Dir* cur = f_child;
	while (cur){
		if (cur->get_name() == name){
			if (prev == NULL){
				if (l_child == f_child){
					l_child = NULL;
				}
				f_child = cur->sibling;
				cur->par = NULL;
				cur->sibling = NULL;
				return cur;
			}
			else if (cur == l_child){
				prev->sibling = NULL;
				l_child = prev;
				cur->sibling = NULL;
				cur->par = NULL;
				return cur;
			}
			else{
				prev->sibling = cur->sibling;
				cur->sibling = NULL;
				cur->par = NULL;
				return cur;
			}
		}
		else{
			prev = cur;
			cur = cur->sibling;
		}
	}
	return NULL;
}


void Dir::print_all_children() const{
	Dir* cur = f_child;
	while (cur){
		cout << cur->get_name() << endl;
		cur = cur->sibling;
	}
}

Dir* Dir::return_parent() const{
	return par;
}

Dir* Dir::cpy_from(Dir* source){
	if (!source){
		return NULL;
	}
	Dir* cur = new Dir(source->get_name());
	cpy_files_in_dor(cur,source);
	cur -> par = NULL;
	Dir* src = source;
	src = src->f_child;
	if (src){
		cur->f_child = new Dir(src->get_name());
		cpy_files_in_dor(cur->f_child, src);
		cur->f_child->par = cur;
		cur = cur->f_child;
	}
	else{
		return cur;
	}

	while (true){
		if (src->f_child != NULL){
			src = src->f_child;
			cur->f_child = new Dir(src->get_name());
			cpy_files_in_dor(cur->f_child, src);
			cur->f_child->par = cur;
			cur = cur->f_child;
		}
		else{
			if (src->sibling != NULL){
				src = src->sibling;
				cur->sibling = new Dir(src->get_name());
				cpy_files_in_dor(cur->sibling, src);
				cur->sibling->par = cur->par;
				cur->par->l_child = cur->sibling;
				cur = cur->sibling;
			}
			else{
				bool flag = true;
				while (flag){
					cur = cur->par;
					if (cur->par){
						src = src->par;
					}
					else{
						flag = false;
						break;
					}
					if (src->sibling){
						src = src->sibling;
						cur->sibling = new Dir(src->get_name());
						cpy_files_in_dor(cur->sibling, src);
						cur->sibling->par = cur->par;
						cur->par->l_child = cur->sibling;
						cur = cur->sibling;
						break;
					}

				}
				if (!flag){
					break;
				}
			}
		}
	}
	return cur;
}

void Dir::cpy_files_in_dor(Dir*& ptr, Dir* src){
	int size = src->get_count_of_files();
	for (int i = 0; i < size; i++){
		string name = src->files.get_at(i)->get_name();
		File* file = new File(name);
		ptr->files.add(file);
		ptr->d_info.cnt_of_files++;
	}
}


Dir* Dir::return_last_child() const{
	return l_child;
}

DInfo Dir::get_info() const{
	return d_info;
}
void Dir::set_info(const DInfo& info){
	d_info = info;
}

void Dir::set_first_child_info(int n, int s){
	d_info.c_number = n;
	d_info.c_sector = s;
}

void Dir::set_sibling_info(int n, int s){
	d_info.s_number = n;
	d_info.s_sector = s;
}

Dir* Dir::get_par_or_sibling(const string& name,char& c, int& n, int& s){
	Dir* p = this;
	if (p->f_child->get_name() == name){
		c = FIRST_CHILD_SIGN_IN_FILE;
		if (p->f_child->sibling){
			n = p->f_child->sibling->get_cnt();
			s = p->f_child->sibling->get_sector();
			return p;
		}
		else{
			n = INVALID;
			s = INVALID;
			return p;
		}
	}
	p = p->f_child;
	while (true){
		if (p == NULL){
			return p;
		}
		if (p->sibling->get_name() == name){
			c = SIBLING_SIGN_IN_FILE;
			if (p->sibling->sibling){
				n = p->sibling->sibling->get_cnt();
				s = p->sibling->sibling->get_sector();
			}
			else{
				n = INVALID;
				s = INVALID;
			}

			return p;
		}
		p = p->sibling;
	}
	return NULL;
}

Dir* Dir::return_f_child() const{
	return f_child;
}

Dir* Dir::rturn_sibling() const{
	return sibling;
}

void Dir::del_p(Dir* p){
	if (!p){
		return;
	}
	Dir* s = p->f_child;
	while (s){
		Dir* x = s;
		s = x->sibling;
		del_p(x);
	}

	delete p;
}

void Dir::add_file(const string& name,int n,int s,long long unsigned size){
	File* file = new File(name);
	file->set_number(n,s);
	file->set_size(size);
	files.add(file);
}

void Dir::remove_file(const string& name){
	int size = files.get_len();
	for (int i = 0; i < size; i++){
		if (files.get_at(i)->get_name() == name){
			delete files.get_at(i);
			files.set_at(i, NULL);
			files.removeNull();
		}
	}
}

void Dir::change_file_name(const string& which_file, const string& name){
	int size = files.get_len();
	for (int i = 0; i < size; i++){
		if (files.get_at(i)->get_name() == which_file){
			files.get_at(i)->set_name(name);
		}
	}
}

int Dir::get_file_number_by_name(const string& name,char c){
	int size = files.get_len();
	for (int i = 0; i < size; i++){
		if (files.get_at(i)->get_name() == name){
			if (c == 'n'){
				return files.get_at(i)->get_cnt();
			}
			if (c == 's'){
				return files.get_at(i)->get_sector();
			}
		}
	}
	return -1;
}

long long unsigned Dir::get_file_size_by_name(const string& name){
	int size = files.get_len();
	for (int i = 0; i < size; i++){
		if (files.get_at(i)->get_name() == name){
			return files.get_at(i)->get_size();
		}
	}
	return -1;
}


void Dir::print_all_files() const{
	int size = files.get_len();
	for (int i = 0; i < size; i++){
		cout << files.get_at(i)->toString() << endl;
	}
}

size_t Dir::get_count_of_files() const{
	return files.get_len();
}

void Dir::set_file_size_by_name(const string& name, long long unsigned size){
	int cnt = files.get_len();
	for (int i = 0; i < cnt; i++){
		files.get_at(i)->set_size(size);
	}
}