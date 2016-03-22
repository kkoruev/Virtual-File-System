 #include "File_system.h"
#include "Functions.h"
#include <string>
#include <fstream>



using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::ofstream;
using std::ifstream;
using std::fstream;

bool File_system::load_file(const string& name_of_file){
	bool flag = file.open(name_of_file);
	if (!flag){
		return flag;
	}

	flag = file.read_from_start(&information, sizeof(information));
	if (!flag){
		return flag;
	}

	root = load_dir_from_file(information.root_dir_node, information.root_dir_sector);
	if (!root){
		return false;
	}
	cur = root;
	root->get_info().print();
	Dir* p = cur;

	while (true){
		DInfo infod = p->get_info();
		if (infod.c_number != INVALID){
			Dir* temp = load_dir_from_file(infod.c_number , infod.c_sector);
			if (!temp){
				return false;
			}
			p->add_dir_by_ptr(temp,temp->get_cnt(),temp->get_sector());
			p = temp;
			continue;
		}
		if (infod.s_number != INVALID){
			Dir* temp = load_dir_from_file(infod.s_number, infod.s_sector);
			if (!temp){
				return false;
			}
			Dir* par = p->return_parent();
			if (!par){
				return false;
			}
			par->add_dir_by_ptr(temp, temp->get_cnt(), temp->get_sector());
			p = temp;
			continue;
		}
		Dir* h = p->return_parent();
		if (!h){
			break;
		}
		p = h;
		bool flag = true;
		while (true){
			infod = p->get_info();
			if (infod.s_number != INVALID){
				Dir* temp = load_dir_from_file(infod.s_number, infod.s_sector);
				if (!temp){
					return false;
				}
				Dir* par = p->return_parent();
				if (!par){
					return false;
				}
				par->add_dir_by_ptr(temp, temp->get_cnt(), temp->get_sector());
				p = temp;
				break;
			}
			else{
				Dir* par = p->return_parent();
				if (!par){
					flag = false;
					break;
				}
				p = par;
			}
		}
		if (!flag){
			break;
		}
	}

	return true;
}

Dir* File_system::load_dir_from_file(int n, int s){
	int block;
	bool flag = file.read(n, s, &block, sizeof(block));
	if (!flag){
		return NULL;
	}
	DInfo d_info;
	flag = file.read(block, &d_info, sizeof(d_info));
	if (!flag){
		return NULL;
	}
	char name[MAX_LEN_NAME_DIR];
	flag = file.read(name, MAX_LEN_NAME_DIR);
	name[MAX_LEN_NAME_DIR - 1] = '\0';
	string n_name(name);
	Dir* temp = new Dir(n_name);
	temp->set_number(n, s);
	temp->set_info(d_info);
	int f_cnt = d_info.cnt_of_files;
	int file_number, file_sector;
	for (int i = 0; i < f_cnt; i++){
		flag = file.read(&file_number, sizeof(file_number));
		if (!flag){
			return NULL;
		}
		flag = file.read(&file_sector, sizeof(file_number));
		if (!flag){
			return NULL;
		}
		long long unsigned last_place = file.get_read_position();
		flag =  load_file_from_file_system(temp,file_number, file_sector);
		if (!flag){
			return NULL;
		}
		file.go_to(last_place);
	}
	return temp;
}

bool File_system::load_file_from_file_system(Dir* ptr,int n, int s){
	int block;
	bool flag = file.read(n, s, &block, sizeof(block));
	if (!flag){
		return false;
	}
	int sign_file;
	flag = file.read(block, &sign_file, sizeof(sign_file));
	if (!flag){
		return false;
	}
	long long unsigned size;
	flag = file.read(&size, sizeof(size));
	if (!flag){
		return false;
	}

	char name[MAX_LEN_NAME_DIR];
	flag = file.read(name, MAX_LEN_NAME_DIR);
	if (!flag){
		return false;
	}
	name[MAX_LEN_NAME_DIR - 1] = '\0';
	string name_f(name);
	ptr->add_file(name_f, n, s,size);
	return flag;
}



void File_system::start(){
	int choice = print_start_text();
	if (choice == 1){
		string name_of_file;
		do{
			cout << "Enter the name of the file system" << endl;
			cin >> name_of_file;
		} while (!load_file(name_of_file));
	}
	else{
		string name_of_file;
		string name_of_main_dir;
		cout << "Enter the name of the file system" << endl;
		cin >> name_of_file;
		cout << "Enter the name of the main dir" << endl;
		cin >> name_of_main_dir;
		start_file_system(name_of_file,name_of_main_dir);
	}
	while(true){
		cout << "> ";
		string text;
		cin.clear();
		cin.sync();
		std::getline(cin, text);
		Darray<string> tokens = tokenize(text,' ');
		bool flag;
		if (tokens.get_len() <= 1){
			if (tokens.get_len() == 1){
				flag = execute_single_command(tokens);
			}
		}
		else{
			 flag = execute_command(tokens);
		}
		if (!flag){
			cout << "There is problem with your command " << endl;
		}
		else{
			cout << "Command executed!" << endl;
		}
	}
	cout << endl;
}

bool File_system::execute_command(const Darray<string>& tokens){
	bool status = false;
	if (tokens.get_at(0) == "mkdir"){
		status = create_dir(tokens.get_at(1));
	}
	else if (tokens.get_at(0) == "rmdir"){
		status = remove_dir(tokens.get_at(1));
	}
	else if (tokens.get_at(0) == "cdname"){	
		if (tokens.get_len() != 3){
			return false;
		}
		status = change_dir_name(tokens.get_at(1), tokens.get_at(2));
	}
	else if (tokens.get_at(0) == "mvdir"){
		if (tokens.get_len() != 3){
			return false;
		}
		status = move_dir(tokens.get_at(1), tokens.get_at(2));
	}
	else if (tokens.get_at(0) == "cpydir"){
		if (tokens.get_len() != 3){
			return false;
		}
		status = copy_dir(tokens.get_at(1), tokens.get_at(2));
	}

	else if (tokens.get_at(0) == "mkfile"){
		status = create_file(tokens.get_at(1));
	}
	else if (tokens.get_at(0) == "rmfile"){
		status = remove_file(tokens.get_at(1));
	}
	else if (tokens.get_at(0) == "cdfile"){
		if (tokens.get_len() != 3){
			return false;
		}
		status = change_file_name(tokens.get_at(1), tokens.get_at(2));
	}
	else if (tokens.get_at(0) == "mvfile"){
		if (tokens.get_len() != 3){
			return false;
		}
		status = move_file(tokens.get_at(1), tokens.get_at(2));
	}
	else if (tokens.get_at(0) == "cpyfile"){
		if (tokens.get_len() != 3){
			return false;
		}
		status = move_file(tokens.get_at(1), tokens.get_at(2));
	}
	else if (tokens.get_at(0) == "finfo"){
		status = info_file(tokens.get_at(1));
	}
	else if (tokens.get_at(0) == "in"){
		Dir * p =  cur->find_child_by_name(tokens.get_at(1));
		if (p){
			cur = p;
			return true;
		}
		else{
			return false;
		}
	}
	else if (tokens.get_at(0) == "append"){
		string text;
		if (!check_tokens_for_append(text, tokens) ){
			return false;
		}
		int size = tokens.get_len();
		status = apend_text(tokens.get_at(size - 1), text);
	}
	else if (tokens.get_at(0) == "exportF"){
		status = export_file(tokens.get_at(1), tokens.get_at(2));
	}
	else if (tokens.get_at(0) == "importF"){
		status = import_file(tokens);
	}
	return status;
}

string File_system::text_for_append(){
	string text;
	for (int i = 0; i < 1500; i++){
		text.push_back('a');
	}
	return text;
}

bool File_system::execute_single_command(const Darray<string>& tokens){
	if (tokens.get_at(0) == "listd"){
		dir_info();
		return true;
	}
	if (tokens.get_at(0) == "back"){
		Dir* p = cur->return_parent();
		if (p){
			cur = p;
			return true;
		}
	}
	return false;
} 

bool File_system::start_file_system(const string& name_of_file,const string&  name_of_main_dir){
	bool flag = file.create(name_of_file);
	if (!flag){
		return flag;
	}

	root = new Dir(name_of_main_dir);
	root->set_number(information.first_node_to_use, information.sector);
	cur = root;

	DInfo dirI = root->get_info();
	flag = file.write(information.first_block_to_use, &dirI, sizeof(dirI));
	if (!flag){
		return flag;
	}
	flag = file.write(name_of_main_dir.c_str(), 120);
	if (!flag){
		return flag;
	}
	flag = file.write(&END_SIGN, sizeof(int));
	if (!flag){
		return flag;
	}

	flag = file.write(information.first_node_to_use, information.sector, &information.first_block_to_use, sizeof(information.first_block_to_use));
	if (!flag){
		return flag;
	}
	flag = file.write(&INVALID, sizeof(INVALID));
	if (!flag){
		return flag;
	}


	information.root_dir_node = information.first_block_to_use;
	information.root_dir_sector = information.sector;
	information.count_of_used_blocks++;
	information.first_block_to_use++;
	information.first_node_to_use++;

	flag = file.write_from_start(&information, sizeof(information));
	if (!flag){
		return flag;
	}
	return flag;
}

void File_system::init_info(){
	information.sign = -1;
	information.first_block = first_block;
	information.first_block_to_use = first_block_to_use;
	information.max_inodes = max_inodes;
	information.count_of_used_blocks = count_of_used_blocks;
	information.sector = 1;
	information.next = next;
	information.first_node_to_use = 0;
}




int File_system::print_start_text(){
	cout << "Hey! You can start your file system now !" << endl;
	cout << "Press (1) if you want to load your old fike system" << endl;
	cout << "Press (2) if you want to create a new one" << endl;
	cout << "Pressing other button will make you choose again" << endl;
	int choice;
	do{
		cin >> choice;
	} while (choice != 1 && choice != 2);
	return choice;
}

bool File_system::create_dir(const string& name){

	if (!add_new_dir_file(name)){
		return false;
	}
	cur->add_new_dir(name, information.first_node_to_use-1, information.sector);


	bool flag = file.write_from_start(&information, sizeof(information));

	return flag;
}

bool File_system::add_new_dir_file(const string& name){

	Dir* last_child = cur->return_last_child();
	if (!last_child){
		bool flag = save_dir_in_file_helper(cur, name,FIRST_CHILD_SIGN_IN_FILE);
		return flag;
	}

	bool flag = save_dir_in_file_helper(last_child, name,SIBLING_SIGN_IN_FILE);
	return flag;

	
}

bool File_system::save_dir_in_file_helper(Dir* ptr,const string& name,char c){
	int number_of_block = find_main_block(ptr->get_cnt(), ptr->get_sector());
	if (number_of_block == -1){
		return false;
	}
	int n, s;
	init_number_and_sector(n, s);
	int place = make_inode(n, s);
	if (place == -1){
		return false;
	}
	DInfo d_info;
	bool flag = file.read(number_of_block, &d_info, sizeof(d_info));
	if (!flag){
		return false;
	}
	if (c == FIRST_CHILD_SIGN_IN_FILE){
		d_info.c_number = n;
		d_info.c_sector = s;
	}
	else{
		d_info.s_number = n;
		d_info.s_sector = s;
	}
	flag = file.write(number_of_block, &d_info, sizeof(d_info));
	if(!flag){
		return false;
	}
	ptr->set_info(d_info);
	flag = save_dir_in_file(place, name);
	return flag;
}

int File_system::make_inode(int n, int s){
	bool flag = file.write(n, s, &information.first_block_to_use, sizeof(int));
	if (!flag){
		return -1;
	}
	flag = file.write(&INVALID, sizeof(INVALID));
	if (!flag){
		return -1;
	}
	int place = information.first_block_to_use++;
	information.count_of_used_blocks++;
	flag = file.write_from_start(&information, sizeof(information));
	if (!flag){
		return -1;
	}
	return place;
}

bool File_system::save_dir_in_file(int place, const string& name){
	DInfo d_info;
	bool flag = file.write(place, &d_info, sizeof(d_info));
	if (!flag){
		return false;
	}
	flag = file.write(name.c_str(), MAX_LEN_NAME_DIR);
	if (!flag){
		return flag;
	}
	flag = file.write(&END_SIGN, sizeof(END_SIGN));
	return flag;
}

int File_system::find_main_block(int n, int s){
	int block;
	bool flag = file.read(n, s, &block, sizeof(int));
	if (!flag){
		return -1;
	}
	return block;
}

bool File_system::remove_dir(const string& name){
	bool flag;
	char sign;
	int n, s;
	Dir* p_s = cur->get_par_or_sibling(name,sign,n,s);
	if (p_s == NULL){
		return false;
	}
	Dir * d = cur->prepare_for_move(name);
	if (!d){
		return false;
	}
	change_dir_after_remove(p_s, sign, n, s);
//	invalid_dir(d);
	delete d;
    throw "ss";
	return true;
}

void File_system::invalid_dir(Dir* ptr){
	Dir* temp = ptr;
	if (!temp){
		return;
	}
	int block;
	int block_next;
	bool flag = file.read(ptr->get_cnt(), ptr->get_sector(), &block, sizeof(block));
	flag = file.read(&block_next, sizeof(block_next));
	flag = file.write(ptr->get_cnt(), ptr->get_sector(), &INVALID, sizeof(INVALID));
	if (block_next != INVALID){
		//..
	}
	flag = file.write(block, &END_SIGN, sizeof(END_SIGN));
	firsT_unused_block++;
	flag = file.write_n_bytes(FIRST_UN_B + firsT_unused_block * sizeof(int), &block, sizeof(block));
	int n = ptr->get_cnt();
	int s = ptr->get_sector(); 
	first_unused_node++;
	flag = file.write_n_bytes(FIRST_UN_N + first_unused_node * sizeof(int), &n, sizeof(n));
	first_unused_node++;
	flag = file.write( &s, sizeof(n));
	if (temp->return_f_child() != NULL){
		temp = temp->return_f_child();
	}
	else if (temp->rturn_sibling() != NULL){
		temp = temp->rturn_sibling();
	}
	else{
		return;
	}
	invalid_dir(temp);
}

int File_system::get_free_block(){
	int place = information.first_block_to_use;
	information.first_block_to_use++;
	bool flag = file.write_from_start(&information, sizeof(information));
	if (!flag){
		return -1;
	}
	return place;
}

bool File_system::change_dir_name(const string& which_dir, const string& name){
	Dir* n;
	n = cur-> find_child_by_name(which_dir);
	if (!n){
		return false;
	}
	n->change_name(name);
	bool flag = change_dir_name_in_file(n->get_cnt(),n->get_sector(),name);
	return flag;
}


bool File_system::change_dir_name_in_file(int n, int s, const string& name){
	int block;
	bool flag = file.read(n, s, &block, sizeof(block));
	if (!flag){
		return false;
	}
	DInfo info;
	flag = file.read(block, &info, sizeof(info));
	if (!flag){
		return flag;
	}
	flag = file.write(block, &info, sizeof(info));
	if (!flag){
		return false;
	}
	flag = file.write(name.c_str(), MAX_LEN_NAME_DIR);
	file.flush();
	return flag;
}

bool File_system::move_dir(const string& which_dir, const string& path){
	Dir* p = find_dir_by_path(path);
	if (!p){
		return false;
	}
	char sign;
	int n;
	int s;
	Dir* p_s = cur->get_par_or_sibling(which_dir, sign, n, s);
	if (p_s == NULL){
		return false;
	}
	Dir* temp;
	temp =  cur-> prepare_for_move(which_dir);
	if (!temp){
		return false;
	}
	DInfo info = temp->get_info();
	info.s_number = INVALID;
	info.s_sector = INVALID;
	temp->set_info(info);

	change_dir_after_remove(p_s, sign, n, s);


	Dir* l_child = p->return_last_child();
	cout << temp->get_cnt() << endl;
	cout << temp->get_sector() << endl;
	if (!l_child){
		bool flag = save_dir_in_file_after_move(p, temp->get_cnt(),temp->get_sector(),info, FIRST_CHILD_SIGN_IN_FILE);
	}
	else{
		bool flag = save_dir_in_file_after_move(l_child, temp->get_cnt(), temp->get_sector(),info, SIBLING_SIGN_IN_FILE);
	}
	int test_block;
	file.read(p->get_cnt(), p->get_sector(), &test_block, sizeof(test_block));

	p -> add_dir_by_ptr(temp,temp->get_cnt(),temp->get_sector());

	return true;
}


bool File_system::save_dir_in_file_after_move( Dir* ptr, int n, int s,DInfo info, char c){
	int block_for_moving_dir;
	bool flag = file.read(n, s, &block_for_moving_dir, sizeof(block_for_moving_dir));
	if (!flag){
		return flag;
	}
	flag = file.write(block_for_moving_dir, &info, sizeof(info));
	if (!flag){
		return flag;
	}
	int main_block;
	flag = file.read(ptr->get_cnt(), ptr->get_sector(), &main_block, sizeof(main_block));
	if (!flag){
		return false;
	}
	DInfo temp_info;
	if (c == FIRST_CHILD_SIGN_IN_FILE){
		flag = file.read(main_block, &temp_info, sizeof(temp_info));
		temp_info.c_number = n;
		temp_info.c_sector = s;
		flag = file.write(main_block, &temp_info, sizeof(temp_info));
		ptr->set_info(temp_info);
		file.flush();
	}
	else{
		flag = file.read(main_block, &temp_info, sizeof(temp_info));
		temp_info.s_number = n;
		temp_info.s_sector = s;
		flag = file.write(main_block, &temp_info, sizeof(temp_info));
		ptr->set_info(temp_info);
		file.flush();
	}
	return flag;
}

bool File_system::change_dir_after_remove(Dir* p_s, char sign, int n, int s){

	// testl;
	// end test
	bool flag;
	DInfo p_s_info = p_s->get_info();
	if (sign == FIRST_CHILD_SIGN_IN_FILE){
		int block = find_main_block(p_s->get_cnt(), p_s->get_sector());
		if (block == -1){
			return false;
		}
		p_s_info.c_number = n;
		p_s_info.c_sector = s;
		p_s->set_info(p_s_info);
		flag = file.write(block, &p_s_info, sizeof(p_s_info));
		if (!flag){
			return flag;
		}
		DInfo x;
		file.read(block, &x, sizeof(x));
	}
	else{
		int block = find_main_block(p_s->get_cnt(), p_s->get_sector());
		if (block == -1){
			return false;
		}
		p_s_info.s_number = n;
		p_s_info.s_sector = s;
		p_s->set_info(p_s_info);
		flag = file.write(block, &p_s_info, sizeof(p_s_info));
		if (!flag){
			return flag;
		}
		DInfo x;
		file.read(block, &x, sizeof(x));
	}
	return true;
}

bool File_system::copy_dir(const string& which_dir, const string& path){
	Dir* n;
	n = cur->find_child_by_name(which_dir);
	if (!n){
		return false;
	}
	Dir* p = find_dir_by_path(path);
	if (!p){
		return false;
	}
	Dir* temp = NULL;
	temp = temp->cpy_from(n);
	if (!temp){
		return false;
	}
	int number, sector;
	init_number_and_sector(number, sector);
	temp->add_dir_by_ptr(temp, number, sector);
	return true;
}

bool File_system::dir_info(){
	cur->print_all_children();
	cur->print_all_files();
	return true;
}

bool File_system::create_file(const string& name){
	int n, s;
	if (!init_number_and_sector(n, s)){
		return false;
	}
	cur->add_file(name, n, s);
	bool flag = save_file_in_file_system(name,n,s);
	if (!flag){
		return flag;
	}
	flag = save_file_in_dir_in_file_system(cur->get_cnt(), cur->get_sector(), n, s);

	return flag;
}

bool File_system::save_file_in_file_system(const string& name, int n, int s){
	int  block = get_free_block();
	bool flag = file.write(n, s, &block, sizeof(block));
	if (!flag){
		return false;
	}
	flag = file.write(&INVALID, sizeof(INVALID));
	if (!flag){
		return false;
	}
	flag = file.write(block, &FILES, sizeof(FILES));
	if (!flag){
		return false;
	}
	int size = EMPTY_FILE;
	flag = file.write(&EMPTY_FILE, sizeof(EMPTY_FILE));
	if (!flag){
		return false;
	}
	flag = file.write(name.c_str(), MAX_LEN_NAME_DIR);
	file.flush();
	return flag;
}

bool File_system::save_file_in_dir_in_file_system(int dir_n, int dir_s, int f_n, int f_s){
	int dir_block;
	bool flag = file.read(dir_n, dir_s, &dir_block, sizeof(dir_block));
	if (!flag){
		return false;
	}
	DInfo info = cur->get_info();
	if (info.cnt_of_files == MAX_COUNT_OF_FILES_IN_BEG_DIR){
		return false;
	}
	int pos = next_file_pos(info.cnt_of_files);
	info.cnt_of_files++;
	flag  = file.write(dir_block, &info, sizeof(info));
	if (!flag){
		return false;
	}
	cur->set_info(info);
	flag = file.write_after_n_bytes(pos, &f_n, sizeof(f_n));
	if (!flag){
		return flag;
	}
	flag = file.write(&f_s, sizeof(f_s));
	file.flush();
	return flag;

}

int File_system::next_file_pos(int n) const{
	int next = MAX_LEN_NAME_DIR + n*sizeof(int)+n*sizeof(int);
	return next;
}

bool File_system::remove_file(const string& name){
	//
	char number = 'n';
	char sector = 's';
	int n = cur->get_file_number_by_name(name, number);
	if (n == -1){
		return false;
	}
	int s = cur->get_file_number_by_name(name, sector);
	if (s == -1){
		return false;
	}

	bool flag = remove_file_from_dir(cur->get_cnt(),cur->get_sector(),n,s);
	if (!flag){
		return flag;
	}
	cur -> remove_file(name);
	return true;
}

bool File_system::remove_file_from_dir(int dir_n, int dir_s, int f_n, int f_s){
	int block;
	bool flag = file.read(dir_n, dir_s, &block, sizeof(block));
	if (!flag){
		return flag;
	}
	int next_block;
	flag = file.read(&next_block, sizeof(next_block));
	if (!flag){
		return false;
	}
	else{
		int condition = MAX_COUNT_OF_FILES_IN_BEG_DIR;
		DInfo info = cur->get_info();
		int cnt_of_files = info.cnt_of_files;
		int file_n, file_s;
		long long unsigned place;
		for (int i = 0; i < cnt_of_files; i++){
			if (i == 0){
				file.go_to_block_w(block);
				bool flag = file.read_after_n_bytes(sizeof(info)+MAX_LEN_NAME_DIR, &file_n, sizeof(file_n));
				place = file.get_read_position() - sizeof(int);
				if (!flag){
					return false;
				}
				flag = file.read(&file_s, sizeof(file_s));
				if (!flag){
					return false;
				}
			}
			else{
				place = file.get_read_position();
				flag = file.read(&file_n, sizeof(file_n));
				if (!flag){
					return false;
				}
				flag = file.read(&file_s, sizeof(file_s));
				if (!flag){
					return false;
				}
			}
			if (file_n == f_n && file_s == f_s){
				flag = swap_last_with_cur_in_file(i+2, cnt_of_files, place);
				break;
			}
		}
		info.cnt_of_files--;
		cur->set_info(info);
		file.write(block, &info, sizeof(info));
		file.flush();
	}
	return true;
}

bool File_system::swap_last_with_cur_in_file(int cnt, int cnt_of_files, int place){
	int last_file_n;
	int last_file_s;
	int skip;
	bool flag;
	if ((cnt_of_files - cnt) < 0){
		return true;
	}
	else{
		skip = (cnt_of_files - cnt) * 2 * sizeof(int);
		flag = file.read_after_n_bytes(skip, &last_file_n, sizeof(last_file_n));
		if (!flag){
			return flag;
		}
	}

	flag = file.read(&last_file_s, sizeof(last_file_s));
	if (!flag){
		return flag;
	}
	file.go_to_w(place);
	flag = file.write(&last_file_n, sizeof(last_file_n));
	if (!flag){
		return flag;
	}
	flag = file.write(&last_file_s, sizeof(last_file_s));
	file.flush();
	return flag;
}


bool File_system::change_file_name(const string& which_file, const string& name){
	char number = 'n';
	char sector = 's';
	int n = cur->get_file_number_by_name(which_file, number);
	if (n == -1){
		return false;
	}
	int s = cur->get_file_number_by_name(which_file, sector);
	if (s == -1){
		return false;
	}
	cur->change_file_name(which_file, name);
	bool flag = change_file_name_in_file_system(name,n,s);
	return flag;
}

bool File_system::change_file_name_in_file_system(const string& name, int n, int s){
	int block;
	bool flag = file.read(n, s, &block, sizeof(block));
	if (!flag){
		return false;
	}
	int sign = FILES;
	flag = file.write(block, &sign, sizeof(sign));
	if (!flag){
		return false;
	}
	flag = file.write_after_n_bytes(sizeof(int), name.c_str(), MAX_LEN_NAME_DIR);
	file.flush();
	return flag;
}

bool File_system::move_file(const string& which_file, const string& path){
	Dir* p = find_dir_by_path(path);
	if (!p){
		return false;
	}
	char number = 'n';
	char sector = 's';
	char size = 'c';
	int n = cur->get_file_number_by_name(which_file, number);
	if (n == -1){
		return false;
	}
	int s = cur->get_file_number_by_name(which_file, sector);
	if (s == -1){
		return false;
	};
	int size_of_file = cur->get_file_size_by_name(which_file);
	if (size_of_file == -1){
		return false;
	}
	bool flag = remove_file_from_dir(cur->get_cnt(), cur->get_sector(), n, s);
	if (!flag){
		return flag;
	}
	cur->remove_file(which_file);
	//
	flag = save_file_after_remove_in_new_dir(p,n,s);
	if (!flag){
		return false;
	}
	p->add_file(which_file, n, s, size_of_file);
	return true;
}

bool File_system::save_file_after_remove_in_new_dir(Dir* p, int n, int s){
	int dir_n = p->get_cnt();
	int dir_s = p->get_sector();
	int first_block_node;
	bool flag = file.read(dir_n, dir_s, &first_block_node, sizeof(first_block_node));
	if (!flag){
		return false;
	}
	long long unsigned place = first_block_node * BLOCK_SIZE + first_block * BLOCK_SIZE;

	int next_block;
	flag = file.read(&next_block, sizeof(next_block));
	if (!flag){
		return false;
	}
	if (next_block != INVALID){

	}
	else{
		int cnt_of_files = p->get_count_of_files();
		if (cnt_of_files >= MAX_COUNT_OF_FILES_IN_BEG_DIR){

		}
		else{
			file.go_to_w(place);
			flag = file.write_after_n_bytes(sizeof(DInfo) + MAX_LEN_NAME_DIR + cnt_of_files * 2 * sizeof(int), &n, sizeof(n));
			if (!flag){
				return false;
			}
			flag = file.write(&s, sizeof(s));
			if (!flag){
				return false;
			}
		}
	}
	DInfo info = p->get_info();
	info.cnt_of_files++;
	flag = file.write(first_block_node, &info, sizeof(info));
	p->set_info(info);
	file.flush();
	return flag;
}

bool File_system::copy_file(const string& which_file, const string& path){
	// int nct = cur-> get_file_number_by_name(which_file);
	// Dir * p = find_fir_by_path(path);
	// p->add_file(name,number);
	return true;
}

bool File_system::info_file(const string& name_of_file){
	long long unsigned size = cur->get_file_size_by_name(name_of_file);
	if (size == -1){
		return false;
	}
	cout << "size " << size << endl;
	return true;
}


Dir* File_system::find_dir_by_path(const string& path){
	Darray<string> tokens = tokenize(path, '/');
	Dir* cur = root;
	if (tokens.isEmpty()){
		return NULL;
	}
	if (tokens.get_at(0) != cur->get_name()){
		return NULL;
	}
	int size = tokens.get_len();
	for (int i = 1; i < size; i++){
		cur = cur->find_child_by_name(tokens.get_at(i));
		if (!cur){
			return NULL;
		}
	}
	return cur;
}


bool File_system::apend_text(const string& name_of_file, const string& text){
	char number = 'n';
	char sector = 's';
	char size = 'c';
	int n = cur->get_file_number_by_name(name_of_file, number);
	if (n == -1){
		return false;
	}
	int s = cur->get_file_number_by_name(name_of_file, sector);
	if (s == -1){
		return false;
	}
	long long unsigned size_of_file = cur->get_file_size_by_name(name_of_file);
	if (size_of_file == -1){
		return false;
	}
	bool flag = save_text(n, s, size_of_file,text);
	if (!flag){
		return false;
	}
	long long unsigned new_size = size_of_file + text.length();
	flag = save_file_size(n,s,new_size);
	if (!flag){
		return false;
	}
	cur->set_file_size_by_name(name_of_file, new_size);
	test_append(n,s);
	return flag;
}

bool File_system::save_text_helper(const string& text,long long unsigned size,int block,long long unsigned place,int cnt_of_node){
	bool flag;
	int len_of_text = text.length();
	long long unsigned new_size = size + len_of_text;
	int condition;
	if (cnt_of_node == 1){
		condition = MAX_BYTES_IN_FILES_BLOCK_WITH_NAME;
	}
	else{
		condition = MAX_BYTES_IN_FILE_BLOCK;
	}
	if (condition < new_size){
		int free_space = condition - size;
		string first_part = text.substr(0, free_space);
		string second_part = text.substr(free_space);
		flag = save_text_in_file(block, size, first_part,cnt_of_node);
		if (!flag){
			return false;
		}
		if (cnt_of_node == COUNT_OF_MAIN_BLOCKS_IN_NODE){
			int next_n, next_s;
			if (!init_number_and_sector(next_n, next_s)){
				return false;
			}
			int new_block = make_inode(next_n, next_s);
			if (new_block == -1){
				return false;
			}
			file.go_to_w(place);
			flag = file.write(&next_n, sizeof(next_n));
			if (!flag){
				return false;
			}
			flag = file.write(&next_s, sizeof(next_s));
			if (!flag){
				return false;
			}
			long long unsigned new_place = get_next_free_node_place(next_n, next_s);
			if (new_place == -1){
				return false;
			}
			flag = save_text_helper(second_part, EMPTY_FILE, new_block, new_place, 1);
		}
		else{
			int new_block = get_free_block();
			if (new_block == -1){
				return false;
			}
			file.go_to_w(place);
			flag = file.write(&new_block, sizeof(block));
			if (!flag){
				return false;
			}
			long long unsigned new_place = file.get_write_position();
			flag = file.write(&INVALID, sizeof(INVALID));
			if (!flag){
				return false;
			}
			file.go_to(place);
			int block;
			file.read(&block, sizeof(block));
			if (block == 13){
				cout << "222" << endl;
			}
			if (block == 29){
				cout << "NOE " << endl;
			}
			cout << block << endl;
			flag = save_text_helper(second_part, 0, new_block, new_place, cnt_of_node + 1);
			if (!flag){
				return false;
			}
		}
	}
	else{
		flag = save_text_in_file(block, size, text,cnt_of_node);
		if (flag){
			return true;
		}
	}
	return true;
}


long long unsigned File_system::get_next_free_node_place(int n, int s){
	int block;
	bool flag = file.read(n, s, &block, sizeof(block));
	if (!flag){
		return -1;
	}
	if (block == INVALID){
		return -1;
	}
	int next_block;
	long long unsigned place = file.get_read_position();
	flag = file.read(&next_block, sizeof(next_block));
	if (!flag){
		return -1;
	}
	if (next_block != INVALID){
		return -1;
	}
	return place;
}

bool File_system::save_text(int n, int s, long long unsigned size,const string& text){
	int block;
	int second_block;
	bool flag = file.read(n, s, &block, sizeof(block));
	if (!flag){
		return false;
	}
	long long unsigned place = file.get_read_position();
	flag = file.read(&second_block, sizeof(second_block));
	if (!flag){
		return false;
	}
	if (second_block != INVALID){
		int next_block;
		int last_block = second_block;
		long long unsigned new_place;
		int cnt = 2;
		for (int i = 2;i < COUNT_OF_MAIN_BLOCKS_IN_NODE; i++){
			new_place = file.get_read_position();
			flag = file.read(&next_block, sizeof(next_block));
			if (!flag){
				return false;
			}
			if (next_block != INVALID){
				last_block = next_block;
			}
			else{
				break;
			}
			cnt = i + 1;
		}
		if (next_block != INVALID){
			int n_next;
			int s_next;
			new_place = file.get_read_position();
			flag = file.read(&n_next, sizeof(n));
			if (!flag){
				return false;
			}
			if (n_next != INVALID){
				flag = file.read(&s_next, sizeof(s_next));
				if (!flag){
					return false;
				}
				int m_block = find_main_block(n_next, s_next);
				long long unsigned new_size = get_file_size(m_block);
				flag = save_text(n_next, s_next, new_size,text);
				if (!flag){
					return false;
				}
			}
			else{
				long long unsigned new_size = get_file_size(next_block);
				flag = save_text_helper(text, new_size, next_block, new_place, cnt);
				if (!flag){
					return false;
				}
			}
		}
		else{
			long long unsigned new_size = get_file_size(last_block);
			if (new_size == -1){
				return false;
			}
			flag = save_text_helper(text, new_size, last_block, new_place, cnt);
			if (!flag){
				return false;
			}
		}
	}
	else{
		flag = save_text_helper(text, size, block, place,1);
		if (!flag){
			return false;
		}
	}
	return true;
}


bool File_system::init_number_and_sector(int& n, int& s){
	s = information.sector;
	n = information.first_node_to_use++;
	bool flag = file.write_from_start(&information, sizeof(information));
	return flag;
}


long long unsigned File_system::get_file_size(int block){
	long long unsigned size;
	file.go_to_block_w(block);
	bool flag = file.read_after_n_bytes(sizeof(int), &size, sizeof(size));
	if (!flag){
		return -1;
	}
	return size;
}



bool File_system::save_text_in_file(int block, long long unsigned size, const string& text,int cnt){
	bool flag = file.write(block, &FILES, sizeof(FILES));
	if (!flag){
		return false;
	}

	flag = file.write(&size, sizeof(size));
	if (!flag){
		return false;
	}
	int skip;
	if (cnt == 1){
		skip = size + MAX_LEN_NAME_DIR;
	}
	else{
		skip = size;
	}
	flag = file.write_after_n_bytes(skip, text.c_str(),text.length());
	if (!flag){
		return false;
	}
	file.go_to_block_w(block);
	size = size + text.length();
	flag = file.write_after_n_bytes(sizeof(int), &size, sizeof(size));

	return flag;
}


bool File_system::check_tokens_for_append(string& text, const Darray<string>& tokens){
	int size = tokens.get_len();
	string temp = tokens.get_at(1);
	if (temp[0] != '"'){
		return false;
	}
	else{
		temp.erase(0,1);
	}

	if (size == 3){
		temp.erase(temp.length() - 1);
		text.append(temp);
		return true;
	}
	text.append(temp);
	char space = ' ';
	text.push_back(space);
	for (int i = 2; i < size-1; i++){
		temp = tokens.get_at(i);
		int size_st = temp.length();
		if (size_st == 0){
			continue;
		}
		if (temp[size_st - 1] == '"'){
			temp.erase(size_st - 1);
			text.append(temp);
			if (size_st == 1 && i == 1){
				continue;
			}
			return true;
		}
		text.append(temp);
		char space = ' ';
		text.push_back(space);
	}
	return true;
}


bool File_system::save_file_size(int n, int s, long long unsigned new_size){
	int block;
	bool flag = file.read(n, s, &block, sizeof(block));
	if (!flag){
		return false;
	}
	flag = file.write(block, &FILES, sizeof(FILES));
	if (!flag){
		return false;
	}
	flag = file.write(&new_size, sizeof(new_size));
	return flag;
}

void File_system::test_append(int n, int s){
	int block;
	file.read(n, s, &block, sizeof(block));
	int next;
	file.read(&next, sizeof(next));
	if (next != INVALID){
		
	}
	else{
		int sign;
		long long unsigned size;
		file.read(block, &sign, sizeof(sign));
		file.read(&size, sizeof(size));
		char line[MAX_BYTES_IN_FILE_BLOCK+1];
		file.read_after_n_bytes(MAX_LEN_NAME_DIR,line, size);
		if (size > MAX_BYTES_IN_FILE_BLOCK){
			line[MAX_BYTES_IN_FILE_BLOCK] = '\0';
		}
		else{
			line[size] = '\0';
		}

		string text(line);
		cout << text << endl;
	}
}



bool File_system::export_file(const string& name, const string& path){
	char c_number = 'n';
	char c_sector = 's';
	int number, sector;
	bool flag;
	number = cur->get_file_number_by_name(name, c_number);
	if (number == -1){
		return false;
	}
	sector = cur->get_file_number_by_name(name, c_sector);
	if (number == -1){
		return false;
	}
	long long unsigned place = sector * BLOCK_SIZE + number * NODE_SIZE;
	RW_FILE export_file;
	flag = export_file.create(path);
	if (!flag){
		return false;
	}
	char* arr = new char[ARR_SIZE_FOR_EXPORT];

	export_helper(export_file, arr, 1, 0, ARR_SIZE_FOR_EXPORT, place);
	delete[] arr;
	return true;
}

void File_system::test_xport(int n, int s){
	int block;
	file.read(n, s, &block, sizeof(block));
	cout << block << endl;
	for (int i = 0; i < 15; i++){
		file.read(&block, sizeof(block));
		cout << block << endl;
	}
}

bool File_system::export_helper(RW_FILE& export_file, char* arr, int node_cnt, int arr_size, int arr_capacity, long long unsigned place){
	bool flag;
	if (node_cnt == 1){
		file.go_to(place);
		int block;
		flag= file.read(&block, sizeof(block));
		if (!flag){
			return false;
		}
		if (block == INVALID){
			flag = write_array_in_file(export_file, arr, arr_size);
			return flag;
		}
		long long unsigned new_place = file.get_read_position();
		long long unsigned file_size = get_file_size(block);
		int block_size;
		if (file_size > MAX_BYTES_IN_FILES_BLOCK_WITH_NAME){
			block_size = MAX_BYTES_IN_FILES_BLOCK_WITH_NAME;
		}
		else{
			block_size = file_size;
		}
		if ((arr_size + block_size) >= arr_capacity){
			flag = write_array_in_file(export_file,arr,arr_size);
			flag = export_helper(export_file, arr, node_cnt, 0, arr_capacity, place);
		}
		else{
			flag = fill_array(arr,arr_size,block_size,block,node_cnt);
			flag = export_helper(export_file, arr, node_cnt + 1,arr_size, arr_capacity, new_place);
		}
	}
	else if (node_cnt > COUNT_OF_MAIN_BLOCKS_IN_NODE){
		file.go_to(place);
		int n, s;
		flag = file.read(&n, sizeof(n));
		if (!flag){
			return false;
		}
		flag = file.read(&s, sizeof(n));
		if (!flag){
			return false;
		}
		long long unsigned new_place = s*BLOCK_SIZE + n * NODE_SIZE;
		bool flag = export_helper(export_file, arr, 1, arr_size, arr_capacity, new_place);
	}
	else{
		file.go_to(place);
		int block;
		flag = file.read(&block, sizeof(block));
		if (!flag){
			return false;
		}
		if (block == INVALID){
			flag = write_array_in_file(export_file, arr, arr_size);
			return flag;
		}
		long long unsigned new_place = file.get_read_position();
		long long unsigned file_size = get_file_size(block);
		int block_size;
		if (file_size > MAX_BYTES_IN_FILE_BLOCK){
			block_size = MAX_BYTES_IN_FILE_BLOCK;
		}
		else{
			block_size = file_size;
		}
		if ((arr_size + block_size) >= arr_capacity){
			flag = write_array_in_file(export_file, arr, arr_size);
			flag = export_helper(export_file, arr, node_cnt , 0, arr_capacity, place);
		}
		else{
			flag = fill_array(arr, arr_size, block_size, block, node_cnt);
			flag = export_helper(export_file, arr, node_cnt + 1, arr_size, arr_capacity, new_place);
			//	flag = write_array_in_file(export_file, arr, arr_size);
		}
	}
	return true;
}

bool File_system::write_array_in_file(RW_FILE& export_file, char* arr, int arr_size){
	bool flag = export_file.write(arr, arr_size);
	file.flush();
	return flag;
}

bool File_system::fill_array(char* arr, int& arr_size, int block_size, int block, int cnt_node){
	bool flag;
	char* p = arr + arr_size;
	if (cnt_node == 1){
		file.go_to_block_r(block);
		int skip = sizeof(int) + sizeof(EMPTY_FILE) + MAX_LEN_NAME_DIR;
		flag = file.read_after_n_bytes(skip, p, block_size);
	}
	else{
		file.go_to_block_r(block);
		int skip = sizeof(int) + sizeof(EMPTY_FILE); 
		flag = file.read_after_n_bytes(skip, p, block_size);
	}
	arr_size = arr_size + block_size;
	return flag;
}

bool File_system::import_file(const Darray<string> tokens){
	// 
	string name_of_file;
	try{
		name_of_file = get_file_name(tokens);
	}
	catch (std::out_of_range){
		return false;
	}
	catch (std::bad_alloc){
		return false;
	}
	bool flag = create_file(name_of_file);
	if (!flag){
		return false;
	}
	RW_FILE reader;
	string path = create_path(tokens);
	flag = reader.open(path);
	if (!flag){
		return false;
	}
	flag = import_helper(reader,name_of_file);
	return flag;
}

bool File_system::import_helper(RW_FILE& reader, const string& name){
	char number = 'n' ;
	char sector = 's';
	int c_number = cur->get_file_number_by_name(name, number);
	if (c_number == -1){
		return false;
	}
	int c_sector = cur->get_file_number_by_name(name, sector);
	if (c_sector == -1){
		return false;
	}
	long long unsigned place = c_sector* BLOCK_SIZE + c_number*NODE_SIZE;
	long long unsigned size = reader.get_file_size();
	long long unsigned last_size = size;
	reader.go_to(0);

	bool flag = read_and_write_after_import(size, reader, place, 1);
	if (!flag){
		return false;
	}
	file.go_to(place);
	int testblock;
	long long unsigned place1, place2;
	file.read(&testblock, sizeof(testblock));
	file.go_to_block_r(testblock);	
	file.read_after_n_bytes(sizeof(int), &size, sizeof(size));
	place2 = file.get_read_position();

	file.go_to_block_w(testblock);
	file.write_after_n_bytes(sizeof(int), &last_size, sizeof(last_size));
	file.flush();
	place2 = file.get_write_position();
	return flag;
}


bool File_system::read_and_write_after_import(long long unsigned size, RW_FILE& reader, long long unsigned place, int cnt_of_node){
	bool flag;
	char* arr = new char[ARR_SIZE_FOR_IMPORT];
	if (ARR_SIZE_FOR_IMPORT <= size){
		flag = reader.read(arr, ARR_SIZE_FOR_IMPORT);
		if (!flag){
			return false;
		}
		flag = divide_bytes(arr, ARR_SIZE_FOR_IMPORT, place, cnt_of_node);
		if (!flag){
			return false;
		}
		size = size - ARR_SIZE_FOR_IMPORT; // C:/Users/Christopher/Desktop/stupid folder
		try{
			delete arr;
			flag = read_and_write_after_import(size, reader, place, cnt_of_node);
		}
		catch (const char* c){
			cout << c <<  endl;
			cout << place << endl;
		}
		if (!flag){
			return false;
		}
	}
	else{
		flag = reader.read(arr, size);
		if (!flag){
			return false;
		}
		flag = divide_bytes(arr, size, place, cnt_of_node);
		if (!flag){
			return false;
		}
	}
	return true;
}

bool File_system::divide_bytes(char* arr, long long unsigned size, long long unsigned& place, int& cnt_of_node){
	while (true){
		bool flag;
		file.go_to(place);
		int block;
		flag = file.read(&block, sizeof(block));
		if (!flag){
			return false;
		}
		place = file.get_read_position();
		if (cnt_of_node == 1){
			long long unsigned block_size = get_file_size(block);
			if (block_size >= MAX_BYTES_IN_FILES_BLOCK_WITH_NAME){
				throw "ERROR";
			}
			int bytes_to_append = MAX_BYTES_IN_FILES_BLOCK_WITH_NAME - block_size;
			int skip = sizeof(int)+sizeof(EMPTY_FILE)+MAX_LEN_NAME_DIR + block_size;
			if (bytes_to_append > size){
				place = place - 4;
				flag = write_bytes_to_block_after_import(arr, size, block, skip);
				return flag;
			}
			flag = write_bytes_to_block_after_import(arr, bytes_to_append, block, skip);
			if (!flag){
				return false;
			}
			size = size - bytes_to_append;
			cnt_of_node++;
			if (!make_block(place)){
				return false;
			}
			file.go_to(place);
			int next_block;
			file.read(&next_block, sizeof(next_block));
			if (!init_secondary_block_in_file(next_block)){
				return false;
			}
			arr = arr + bytes_to_append;
		}
		else if (cnt_of_node == COUNT_OF_MAIN_BLOCKS_IN_NODE){
			long long unsigned block_size = get_file_size(block);
			if (block_size >= MAX_BYTES_IN_FILE_BLOCK){
				throw "ERROR";
			}
			int bytes_to_append = MAX_BYTES_IN_FILE_BLOCK - block_size;
			int skip = sizeof(int)+sizeof(EMPTY_FILE)+block_size;
			if (bytes_to_append > size){
				place = place - 4;
				flag = write_bytes_to_block_after_import(arr, size, block, skip);
				return flag;
			}
			flag = write_bytes_to_block_after_import(arr, bytes_to_append, block, skip);
			if (!flag){
				return false;
			}



			size = size - bytes_to_append;
			int n, s;
			init_number_and_sector(n, s);
			file.go_to_w(place);
			file.write(&n, sizeof(n));
			file.write(&s, sizeof(s));
			place = s* BLOCK_SIZE + n*NODE_SIZE;
			if (!make_block(place)){
				return false;
			}
			file.go_to(place);
			int next_block;
			file.read(&next_block, sizeof(next_block));
			if (!init_secondary_block_in_file(next_block)){
				return false;
			}
			cnt_of_node = 1;
			arr = arr + bytes_to_append;
		}
		else{
			long long unsigned block_size = get_file_size(block);
			if (block_size >= MAX_BYTES_IN_FILE_BLOCK){
				throw "ERROR";
			}

			int bytes_to_append = MAX_BYTES_IN_FILE_BLOCK - block_size;
			int skip = sizeof(int)+sizeof(EMPTY_FILE)+block_size;
			if (bytes_to_append > size){
				place = place - 4;
				flag = write_bytes_to_block_after_import(arr, size, block, skip);
				return flag;
			}
			flag = write_bytes_to_block_after_import(arr, bytes_to_append, block, skip);
			if (!flag){
				return false;
			}
			size = size - bytes_to_append;
			cnt_of_node++;
			if (!make_block(place)){
				return false;
			}
			file.go_to(place);
			int next_block;
			file.read(&next_block, sizeof(next_block));
			if (!init_secondary_block_in_file(next_block)){
				return false;
			}
			arr = arr + bytes_to_append;
		}
	}
	
	return true;
}


bool File_system::make_block(long long unsigned place){
	int block = get_free_block();
	file.go_to_w(place);
	bool flag = file.write(&block, sizeof(block));
	if (!flag){
		return false;
	}
	flag = file.write(&INVALID, sizeof(INVALID));
	file.flush();
	return flag;
}

bool File_system::init_secondary_block_in_file(int block){
	file.go_to_block_w(block);
	bool flag = file.write(&FILES, sizeof(FILES));
	if (!flag){
		return false;
	}
	flag = file.write(&EMPTY_FILE, sizeof(EMPTY_FILE));
	return flag;
}

bool File_system::write_bytes_to_block_after_import(char* arr, int bytes_to_append, int block, int skip){
	file.go_to_block_r(block);
	long long unsigned size;
	bool flag = file.read_after_n_bytes(sizeof(int), &size, sizeof(size));
	if (!flag){
		return false;
	}
	size = size + bytes_to_append;
	file.go_to_block_w(block);
	flag = file.write_after_n_bytes(sizeof(int), &size, sizeof(size));
	if (!flag){
		return false;
	}
	file.flush();
	file.go_to_block_w(block);
	flag = file.write_after_n_bytes(skip, arr, bytes_to_append);
	file.flush();
	return flag;
}