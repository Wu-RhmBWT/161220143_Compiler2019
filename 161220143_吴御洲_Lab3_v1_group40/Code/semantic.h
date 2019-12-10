#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntaxtree.h"
#include "IR.h"

#define HASH_MAX_SIZE 65536
#define INT_TYPE 0
#define FLOAT_TYPE 1

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

typedef enum Kind_ { BASIC, ARRAY, STRUCTURE, FUNCTION } Kind;

struct Type_ {
	Kind kind;
	union
	{
		//basic type
		int basic;
		//array information
		struct {
			Type elem;
			int size; 
		} array;
		//struct information
		FieldList structure;
		//funcion information
		struct {
			Type retType;	//kind of retun value
			FieldList parameters;
			int paraNum;	//number of parameters
		} function;
	} u;
};

struct FieldList_ {
	char* name;	//field name
	Type type;	//field type
	FieldList tail;	//next field
	int offset;	//offset from origin key
};

unsigned int hash_pjw(char *name);
void initHashtable();
int insertTable(FieldList f);//return 0 fail; return 1 success
FieldList searchTable(char *name,int isFunction);
void printTable();

int isSameType(Type type1,Type type2);//return 0 differ; return 1 same
int isSameFieldList(FieldList f1, FieldList f2);//return 0 differ; return 1 same

void traverseTree(Node *root);

//High-level Definitions
void Program(Node *root);
void ExtDefList(Node *root);
void ExtDef(Node *root);
void ExtDecList(Node *n, Type type);

//Specifiers
Type Specifier(Node *root);
Type StructSpecifier(Node *root);

//Declarators
FieldList VarDec(Node *root, Type type, int isFromStruct);
FieldList FunDec(Node *root, Type type);
int countParaNum(FieldList parameters);
FieldList VarList(Node *root);
FieldList ParamDec(Node *root);

//Statements
void CompSt(Node *root, Type type);
void StmtList(Node *root, Type type);
void Stmt(Node *root, Type type);

//Local Definitions
FieldList DefList(Node *root, int isFromStruct);
FieldList Def(Node *root, int isFromStruct);
FieldList DecList(Node *root, Type type, int isFromStruct);
FieldList Dec(Node *root, Type type, int isFromStruct);

//Expressions
Type Exp(Node *root, Operand place);
int Args(Node *root, Operand *ArgList);

void printFun(FieldList f);
void printArgs(FieldList f);

int getSize(Type type);
Type Cond(Node *root, Operand trueLabel, Operand falseLabel);
#endif
