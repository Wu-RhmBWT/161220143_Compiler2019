%locations

%{
	#include "lex.yy.c"
	extern Node* root;
	extern int errorNum;
	int if_print = 1;

	void yyerror(char * str) {
		printf("Error type B at Line %d: Syntax error.\n", yylineno);
		errorNum++;
		if_print = 0;
	}
%}

%union
{
	struct treeNode *tree_node;
}

%token <tree_node> INT
%token <tree_node> FLOAT
%token <tree_node> ID
%token <tree_node> SEMI
%token <tree_node> COMMA
%token <tree_node> ASSIGNOP
%token <tree_node> RELOP
%token <tree_node> PLUS
%token <tree_node> MINUS
%token <tree_node> STAR
%token <tree_node> DIV
%token <tree_node> AND
%token <tree_node> OR
%token <tree_node> DOT
%token <tree_node> NOT
%token <tree_node> TYPE
%token <tree_node> LP
%token <tree_node> RP
%token <tree_node> LB
%token <tree_node> RB
%token <tree_node> LC
%token <tree_node> RC
%token <tree_node> STRUCT
%token <tree_node> RETURN
%token <tree_node> IF
%token <tree_node> ELSE
%token <tree_node> WHILE
%type <tree_node> Program
%type <tree_node> ExtDefList
%type <tree_node> ExtDef
%type <tree_node> ExtDecList
%type <tree_node> Specifier
%type <tree_node> StructSpecifier
%type <tree_node> OptTag
%type <tree_node> Tag
%type <tree_node> VarDec
%type <tree_node> FunDec
%type <tree_node> VarList
%type <tree_node> ParamDec
%type <tree_node> CompSt
%type <tree_node> StmtList
%type <tree_node> Stmt
%type <tree_node> DefList
%type <tree_node> Def
%type <tree_node> DecList
%type <tree_node> Dec
%type <tree_node> Exp
%type <tree_node> Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT
%left LB RB
%left LP RP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program : ExtDefList {
		$$ = createNewTree("Program", @$.first_line, 0, 1, $1);
		root = $$;
		if(if_print == 1)
			;//printTree(root, 0);
	}
	;
ExtDefList : ExtDef ExtDefList { $$ = createNewTree("ExtDefList", @$.first_line, 0, 2, $1, $2); }
	| {	$$ = createNewTree("ExtDefList", @$.first_line, 0, 0); }
	;
ExtDef : Specifier ExtDecList SEMI { $$ = createNewTree("ExtDef", @$.first_line, 0, 3, $1, $2, $3); }
	| Specifier ExtDecList error SEMI {	printf(""); }
	| Specifier SEMI { $$ = createNewTree("ExtDef", @$.first_line, 0, 2, $1, $2); }
	| Specifier error SEMI { printf(""); }
	| Specifier FunDec CompSt {	$$ = createNewTree("ExtDef", @$.first_line, 0, 3, $1, $2, $3); }
	;
ExtDecList : VarDec { $$ = createNewTree("ExtDecList", @$.first_line, 0, 1, $1); }	
	| VarDec COMMA ExtDecList {	$$ = createNewTree("ExtDecList", @$.first_line, 0, 3, $1, $2, $3); }
	;
Specifier : TYPE { $$ = createNewTree("Specifier", @$.first_line, 0, 1, $1); }
	| StructSpecifier {	$$ = createNewTree("Specifier", @$.first_line, 0, 1, $1); }
	;
StructSpecifier : STRUCT OptTag LC DefList RC {	$$ = createNewTree("StructSpecifier", @$.first_line, 0, 5, $1, $2, $3, $4, $5); }
	| STRUCT OptTag LC DefList error RC { printf(""); } 
	| STRUCT Tag { $$ = createNewTree("StructSpecifier", @$.first_line, 0, 2, $1, $2); }
	;
OptTag : ID { $$ = createNewTree("OptTag", @$.first_line, 0, 1, $1); }
	| {	$$ = createNewTree("OptTag", @$.first_line, 0, 0); }
	;
Tag : ID { $$ = createNewTree("Tag", @$.first_line, 0, 1, $1); }
	;
VarDec : ID { $$ = createNewTree("VarDec", @$.first_line, 0, 1, $1); }
	| VarDec LB INT RB { $$ = createNewTree("VarDec", @$.first_line, 0, 4, $1, $2, $3, $4); }
	| VarDec LB INT error RB { printf(""); }
	;
FunDec : ID LP VarList RP {	$$ = createNewTree("FunDec", @$.first_line, 0, 4, $1, $2, $3, $4); }
	| ID LP VarList error RP { printf(""); } 
	| ID LP RP { $$ = createNewTree("FunDec", @$.first_line, 0, 3, $1, $2, $3); }
	| ID LP error RP { printf(""); }
	;
VarList : ParamDec COMMA VarList { $$ = createNewTree("VarList", @$.first_line, 0, 3, $1, $2, $3); } 
	| ParamDec { $$ = createNewTree("VarList", @$.first_line, 0, 1, $1); }
	;
ParamDec : Specifier VarDec { $$ = createNewTree("ParamDec", @$.first_line, 0, 2, $1, $2); }
	;
CompSt : LC DefList StmtList RC { $$ = createNewTree("CompSt", @$.first_line, 0, 4, $1, $2, $3, $4); }
	| LC DefList StmtList error RC { printf(""); }
	;
StmtList : Stmt StmtList { $$ = createNewTree("StmtList", @$.first_line, 0, 2, $1, $2); }
	| {	$$ = createNewTree("StmtList", @$.first_line, 0, 0); }
	;
Stmt : Exp SEMI { $$ = createNewTree("Stmt", @$.first_line, 0, 2, $1, $2); }
	| Exp error SEMI{ printf(""); }
	| CompSt { $$ = createNewTree("Stmt", @$.first_line, 0, 1, $1); }
	| RETURN Exp SEMI {	$$ = createNewTree("Stmt", @$.first_line, 0, 3, $1, $2, $3); }
	| RETURN Exp error SEMI { printf("");}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {	$$ = createNewTree("Stmt", @$.first_line, 0, 5, $1, $2, $3, $4, $5);}
	| IF LP Exp error RP Stmt %prec LOWER_THAN_ELSE { printf(""); }
	| IF LP Exp RP Stmt ELSE Stmt {	$$ = createNewTree("Stmt", @$.first_line, 0, 7, $1, $2, $3, $4, $5, $6, $7); }
	| IF LP Exp error RP Stmt ELSE Stmt { printf(""); }
	| WHILE LP Exp RP Stmt { $$ = createNewTree("Stmt", @$.first_line, 0, 5, $1, $2, $3, $4, $5); }
	| WHILE LP Exp error RP Stmt { printf(""); }
	;
DefList : Def DefList {	$$ = createNewTree("DefList", @$.first_line, 0, 2, $1, $2); } 
	| {	$$ = createNewTree("DefList", @$.first_line, 0, 0); }
	;
Def : Specifier DecList SEMI { $$ = createNewTree("Def", @$.first_line, 0, 3, $1, $2, $3); }
	| Specifier DecList error SEMI { printf(""); }
	;
DecList : Dec {	$$ = createNewTree("DecList", @$.first_line, 0, 1, $1); }
	| Dec COMMA DecList { $$ = createNewTree("DecList", @$.first_line, 0, 3, $1, $2, $3); }
	;
Dec : VarDec { $$ = createNewTree("Dec", @$.first_line, 0, 1, $1); }
	| VarDec ASSIGNOP Exp {	$$ = createNewTree("Dec", @$.first_line, 0, 3, $1, $2, $3); }
	;
Exp : Exp ASSIGNOP Exp { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }
	| Exp AND Exp {	$$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }
	| Exp OR Exp { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }
	| Exp RELOP Exp { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); } 
	| Exp PLUS Exp { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }
	| Exp MINUS Exp { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }
	| Exp STAR Exp { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }	
	| Exp DIV Exp { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }
	| LP Exp RP { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }
	| LP Exp error RP { printf(""); }
	| MINUS Exp { $$ = createNewTree("Exp", @$.first_line, 0, 2, $1, $2); }
	| NOT Exp { $$ = createNewTree("Exp", @$.first_line, 0, 2, $1, $2); }
	| ID LP Args RP { $$ = createNewTree("Exp", @$.first_line, 0, 4, $1, $2, $3, $4); }
	| ID LP Args error RP { printf(""); }
	| ID LP RP { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }
	| ID LP error RP { printf(""); }
	| Exp LB Exp RB { $$ = createNewTree("Exp", @$.first_line, 0, 4, $1, $2, $3, $4); }
	| Exp LB Exp error RB { printf(""); }
	| Exp DOT ID { $$ = createNewTree("Exp", @$.first_line, 0, 3, $1, $2, $3); }
	| ID { $$ = createNewTree("Exp", @$.first_line, 0, 1, $1); }
	| INT { $$ = createNewTree("Exp", @$.first_line, 0, 1, $1); }
	| FLOAT { $$ = createNewTree("Exp", @$.first_line, 0, 1, $1); }
	;
Args : Exp COMMA Args { $$ = createNewTree("Args", @$.first_line, 0, 3, $1, $2, $3); } 
	| Exp { $$ = createNewTree("Args", @$.first_line, 0, 1, $1); }
	;
%%
