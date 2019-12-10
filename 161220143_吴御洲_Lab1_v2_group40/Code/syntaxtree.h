#ifndef __SYNTAXTREE_H__
#define __SYNTAXTREE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct Node
{
	char *name;
	int line_no;
	int type;//1 leaf	0 not leaf	-1 empty
	union
	{
		int value_i;
		double value_f;
		char *value_s;	
	};	
	
	struct Node *FirstChild;//first child node
	struct Node *NextSib;//next sibling
	
};

struct Node *createNewNode(char *name, int line, ...);

struct Node *createNewTree(char *name, int line, int value, int count, ...);

void printTree(struct Node *node, int count);

#endif
