#include "syntaxtree.h"
#include "semantic.h"


extern int yylineno;
extern char *yytext;

extern int yyparse();
extern int yylex();
extern void yyrestart(FILE *f);
extern void yyerror(char *s, ...);

Node *root = NULL;
int errorNum = 0;

int main(int argc, char** argv) {
    if(argc <= 1) {
        return 1;
    }
    FILE* fp = fopen(argv[1],"r");
    if(!fp) {
        perror(argv[1]);
        return 1;
    }
    yylineno=1;
    yyrestart(fp);
    yyparse();
	if(!errorNum) {
		initHashtable();
		initIRList();
		traverseTree(root);
		//printCode();
		writeCode(argv[2]);
		//printTable();
	}
	else
		printf("Cannot translate: Code contains variables or parameters of structure type\n");
	return 0;
}
