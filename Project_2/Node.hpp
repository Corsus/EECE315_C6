#ifndef NODE_H
#define NODE_H

#include "processt.hpp"
#include <stdio.h>
#include <stdlib.h>

class Node
{
public:
	Node();
	Node(processt p, Node* n);
	void setProcess(processt);
	void setNext(Node*);
	processt getProcess();
	Node* getNext();
private:
	processt p;
	Node* n;
};

#endif
