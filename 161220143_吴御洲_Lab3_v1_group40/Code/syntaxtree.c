#include "syntaxtree.h"

Node *createNewNode(char *name, int line, ...) {
	va_list ap;
	va_start(ap, line);

	Node *new_node = malloc(sizeof(Node));
	new_node->name = name;
	new_node->line_no = line;
	new_node->type = 1;
	new_node->FirstChild = NULL;
	new_node->NextSib = NULL;
	//int or float or string
	if(!strcmp(name, "INT"))
		new_node->u.value_i = va_arg(ap, int);
	else if(!strcmp(name, "FLOAT"))
		new_node->u.value_f = va_arg(ap, double);
	else if(!strcmp(name, "ID")||!strcmp(name, "TYPE")||!strcmp(name, "RELOP")) {
		new_node->u.value_s = malloc(32);
		strcpy(new_node->u.value_s, va_arg(ap, char *));
	}
	va_end(ap);
	return new_node;
}

Node *createNewTree(char *name, int line, int value, int count, ...) {
	Node *root = malloc(sizeof(Node));
	root->name = name;
	root->line_no = line;
	root->type = 0;//not leaf
	if(count == 0)//no child node
		root->type = -1;
	root->FirstChild = NULL;
	root->NextSib = NULL;
	root->u.value_i = value;
	
	va_list ap;
	va_start(ap, count);
	//create child node
	for(int i = 0; i < count; i++) {
		Node *new_node = va_arg(ap, Node *);
		if(root->FirstChild == NULL) {//first child
			root->FirstChild = new_node;
		}
		else {//not first child
			Node *insert = root->FirstChild;
			while(insert->NextSib != NULL)
				insert = insert->NextSib;
			insert->NextSib = new_node;
		}
	}
	return root;
}

void printTree(Node *node, int count) {
	if(node->type == -1)//empty
		return;
	for(int i = 0; i < count; i++)//indentation
		printf("  ");
	
	printf("%s", node->name);
	if(node->type == 0)
		printf(" (%d)", node->line_no);
	//lexical unit
	if(!strcmp(node->name, "INT"))
		printf(": %d\n", node->u.value_i);
	else if(!strcmp(node->name, "FLOAT"))
		printf(": %f\n", node->u.value_f);
	else if(!strcmp(node->name, "ID") || !strcmp(node->name, "TYPE"))
		printf(": %s\n", node->u.value_s);
	else
		printf("\n");	
	//print child nodes
	Node *child = node->FirstChild;
	while(child != NULL) {
		printTree(child, count + 1);//add 2 space
		child = child->NextSib;
	}
	return;
}
