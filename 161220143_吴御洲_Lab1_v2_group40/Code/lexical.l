%option yylineno

%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "syntax.tab.h"
	#include "syntaxtree.h"

	extern int if_print;	
	
	int yycolumn = 0;
	#define YY_USER_ACTION \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;
%}
INTD "0"|([1-9][0-9]{0,31})
INTO 0[0-7]+
INTH 0[xX][0-9a-fA-F]+
FLOAT ([1-9][0-9]*\.[0-9]+)|(0\.[0-9]+)
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP ">"|"<"|">="|"<="|"=="|"!="
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE "int"|"float"
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT "struct"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
ID [a-zA-Z_][a-zA-Z0-9_]{0,31}
SPACE [ \t\r\n]
ERROR .
%%
"\n" {
	yycolumn = 1;
}
{INTD} {
	yylval.tree_node = createNewNode("INT", yylloc.first_line, (int)strtol(yytext, NULL, 10)); 
	return INT;
}
{INTO} {
	yylval.tree_node = createNewNode("INT", yylloc.first_line, (int)strtol(yytext, NULL, 8)); 
	return INT;
}
{INTH} {
	yylval.tree_node = createNewNode("INT", yylloc.first_line, (int)strtol(yytext, NULL, 16)); 
	return INT;
}
{FLOAT} {
	yylval.tree_node = createNewNode("FLOAT", yylloc.first_line, atof(yytext)); 
	return FLOAT;
}
{SEMI} {
	yylval.tree_node = createNewNode("SEMI", yylloc.first_line); 
	return SEMI;
}
{COMMA} {
	yylval.tree_node = createNewNode("COMMA", yylloc.first_line);
	return COMMA;
}
{ASSIGNOP} {
	yylval.tree_node = createNewNode("ASSIGNOP", yylloc.first_line); 
	return ASSIGNOP;
}
{RELOP} {
	yylval.tree_node = createNewNode("RELOP", yylloc.first_line); 
	return RELOP;
}
{PLUS} {
	yylval.tree_node = createNewNode("PLUS", yylloc.first_line);
	return PLUS;
}
{MINUS} {
	yylval.tree_node = createNewNode("MINUS", yylloc.first_line);
	return MINUS;
}
{STAR} {
	yylval.tree_node = createNewNode("STAR", yylloc.first_line);
	return STAR;
}
{DIV} {
	yylval.tree_node = createNewNode("DIV", yylloc.first_line); 
	return DIV;
}
{AND} {
	yylval.tree_node = createNewNode("AND", yylloc.first_line);
	return AND;
}
{OR} {
	yylval.tree_node = createNewNode("OR", yylloc.first_line); 
	return OR;
}
{DOT} {
	yylval.tree_node = createNewNode("DOT", yylloc.first_line);
	return DOT;
}
{NOT} {
	yylval.tree_node = createNewNode("NOT", yylloc.first_line);
	return NOT;
}
{TYPE} {
	char * str = malloc(32);
	int i = 0;
	for(i = 0; i < yyleng; i++)
		str[i] = yytext[i];
	str[i] = '\0';
	yylval.tree_node = createNewNode("TYPE", yylloc.first_line, str); 
	return TYPE;
}
{LP} {
	yylval.tree_node = createNewNode("LP", yylloc.first_line); 
	return LP;
}
{RP} {
	yylval.tree_node = createNewNode("RP", yylloc.first_line);
	return RP;
}
{LB} {
	yylval.tree_node = createNewNode("LB", yylloc.first_line); 
	return LB;
}
{RB} {
	yylval.tree_node = createNewNode("RB", yylloc.first_line); 
	return RB;
}
{LC} {
	yylval.tree_node = createNewNode("LC", yylloc.first_line); 
	return LC;
}
{RC} {
	yylval.tree_node = createNewNode("RC", yylloc.first_line); 
	return RC;
}
{STRUCT} {
	yylval.tree_node = createNewNode("STRUCT", yylloc.first_line);
	return STRUCT;
}
{RETURN} {
	yylval.tree_node = createNewNode("RETURN", yylloc.first_line); 
	return RETURN;
}
{IF} {
	yylval.tree_node = createNewNode("IF", yylloc.first_line); 
	return IF;
}
{ELSE} {
	yylval.tree_node = createNewNode("ELSE", yylloc.first_line); 
	return ELSE;
}
{WHILE} {
	yylval.tree_node = createNewNode("WHILE", yylloc.first_line); 
	return WHILE;
}
{ID} {
	char * str = malloc(32);
	int i = 0;
	for(i = 0; i < yyleng; i++)
		str[i] = yytext[i];
	str[i] = '\0';
	yylval.tree_node = createNewNode("ID", yylloc.first_line, str);
	return ID;
}
{SPACE} { }
{ERROR} {
	printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); if_print = 0;
}
%%