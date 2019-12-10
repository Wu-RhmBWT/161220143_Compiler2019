#ifndef __SYNTAXTREE_H__
#define __SYNTAXTREE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct treeNode
{
	char *name;
	int line_no;
	int type;//1 leaf	0 not leaf	-1 empty
	union
	{
		int value_i;
		double value_f;
		char *value_s;	
	} u;
	struct treeNode *FirstChild;//first child node
	struct treeNode *NextSib;//next sibling
	
}Node;

extern Node *root;

Node *createNewNode(char *name, int line, ...);

Node *createNewTree(char *name, int line, int value, int count, ...);

void printTree(Node *node, int count);

#endif
