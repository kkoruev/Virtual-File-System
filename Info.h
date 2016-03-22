#ifndef INFO_H
#define INFO_H

#include <iostream>

#pragma pack(push,1)

using std::cout;
using std::endl;

struct Info{
	int sign;
	size_t max_inodes;
	size_t first_block;
	size_t root_dir_sector;
	size_t root_dir_node;
	size_t count_of_used_blocks;
	size_t first_block_to_use;
	size_t sector;
	size_t first_node_to_use;
	int next;

public:
	void print() const{
		cout << sign << endl;
		cout << max_inodes << endl;
		cout << first_block << endl;
		cout << root_dir_sector << endl;
		cout << root_dir_node << endl;
		cout << count_of_used_blocks << endl;
		cout << first_block_to_use << endl;
		cout << sector << endl;
		cout << first_node_to_use << endl;
		cout << next << endl;
	}
};

#pragma pack(pop)

#endif // !INFO_H
