#ifndef NODES_H
#define NODES_H

struct Nodes{
	size_t  num;
	size_t sector;

	Nodes(size_t num,size_t sector){
		this->num = num;
		this->sector = sector;
	}
};


#endif // !NODES_H
