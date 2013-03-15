#include <stdio.h>
#include <stdlib.h>
#include "processt.hpp"
#include "Node.hpp"

using namespace std;

Node::Node( void )
{
	processt p_temp;
	this->p = p_temp;
	this->n = NULL;
}

Node::Node(processt p_input, Node* n_input)
{
	setProcess(p_input);
	setNext(n_input);
	this->n=NULL; 
}

void Node::setProcess(processt process)
{
	this->p = process;
}
void Node::setNext(Node* node)
{
	this->n = node;
}
processt Node::getProcess()
{
	return this->p;
}
Node* Node::getNext()
{
	return this->n;
}