#ifndef FUNCTIONS_H
#define FUNCTIONS_H


#include "Darray.h"
#include <string>

Darray<std::string> tokenize(const std::string& text,char c);

std::string get_file_name(const Darray<std::string> tokens);
std::string create_path(const Darray<std::string> tokens);
#endif // !FUNCTIONS_H
