#include   "Functions.h"




Darray<std::string> tokenize(const std::string& text,char c){
	Darray<std::string> tokens;
	std::string temp;
	int size = text.length();
	for (int i = 0; i < size; i++){
		if (text[i] == c){
			tokens.add(temp);
			temp = "";
		}
		else{
			temp.push_back(text[i]);
		}
	}
	tokens.add(temp);
	return tokens;
}

std::string get_file_name(const Darray<std::string> tokens){

	std::string temp;
	int size = tokens.get_len();
	for (int i = 1; i < size; i++){
		temp.append(tokens.get_at(i));
	}
	size_t last_backslash_pos = temp.rfind('/');
	return temp.substr(last_backslash_pos + 1);
}

std::string create_path(const Darray<std::string> tokens){
	std::string temp;
	int size = tokens.get_len();
	for (int i = 1; i < size; i++){
		temp.append(tokens.get_at(i));
		temp.push_back(' ');
	}
	temp.erase(temp.length() - 1);
	return temp;
}