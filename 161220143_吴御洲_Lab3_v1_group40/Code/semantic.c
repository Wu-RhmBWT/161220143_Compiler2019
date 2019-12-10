#include "semantic.h"

FieldList hashTable[HASH_MAX_SIZE];

extern int VarCount, LabelCount, TempVarCount;

unsigned int hash_pjw(char *name) {
	unsigned int val = 0, i;
	for( ; *name; ++name) {
		val = (val << 2) + *name;
		if(i = val & ~HASH_MAX_SIZE)
			val = (val ^ (i >> 12)) & HASH_MAX_SIZE;
	}
	return val;
}

void initHashtable() {
	for(int i = 0; i < HASH_MAX_SIZE; i++)
		hashTable[i] = NULL;
	//function read in lab3
	FieldList f1 = malloc(sizeof(struct FieldList_));
	f1->type = malloc(sizeof(struct Type_));
	f1->name = malloc(100);
	strcpy(f1->name, "read");
	f1->tail = NULL;
	f1->type->kind = FUNCTION;
	f1->type->u.function.retType = malloc(sizeof(struct Type_));
	f1->type->u.function.retType->kind = BASIC;
	f1->type->u.function.retType->u.basic = INT_TYPE;
	f1->type->u.function.paraNum = 0;
	f1->type->u.function.parameters = NULL;
	insertTable(f1);
	//function write in lab3
	FieldList f2 = malloc(sizeof(struct FieldList_));
	f2->type = malloc(sizeof(struct Type_));
	f2->name = malloc(100);
	strcpy(f2->name,"write");
	f2->tail = NULL;
	f2->type->kind = FUNCTION;
	f2->type->u.function.retType = malloc(sizeof(struct Type_));
	f2->type->u.function.retType->kind = BASIC;
	f2->type->u.function.retType->u.basic = INT_TYPE;
	f2->type->u.function.paraNum = 1;
	FieldList write_param = malloc(sizeof(struct FieldList_));
	write_param->name = malloc(100);
	strcpy(write_param->name, "num");
	write_param->tail = NULL;
	write_param->type = malloc(sizeof(struct Type_));
	write_param->type->kind = BASIC;
	write_param->type->u.basic = INT_TYPE;
	f2->type->u.function.parameters = write_param;
	insertTable(f2);
}

int insertTable(FieldList f) {
	if(!f)
		return 0;
	if(!f->name)
		return 0;
	f->offset = 0;
	unsigned int key;
	if(f->type->kind == FUNCTION)
	    key = hash_pjw(f->name + 1);
	else
		key = hash_pjw(f->name);
	if(!hashTable[key]) {
		hashTable[key] = f;
		return 1;
	}
	while(1) {
		key = (key + 1) % HASH_MAX_SIZE;
		f->offset++;
		if(!hashTable[key]) {
			hashTable[key] = f;
			return 1;
		}
	}
	return 0;
}

FieldList searchTable(char *name,int isFunction) {
	if(!name)
		return NULL;
	unsigned int key, initial;
	if(isFunction)
	    key = hash_pjw(name + 1);
	else 
		key = hash_pjw(name);
	initial = key;
	FieldList ret = hashTable[key];
	while(ret){
		if(!strcmp(name, ret->name)) {
			if((isFunction)&&(ret->type->kind == FUNCTION))
				return ret;
			if((!isFunction)&&(ret->type->kind != FUNCTION))
				return ret;
		}
		key = (key + 1) % HASH_MAX_SIZE;
		ret = hashTable[key];
	}
	return NULL;
}

void printTable() {
	for(int i = 0; i < HASH_MAX_SIZE; i++) {
		if(hashTable[i]) {
			printf("name:%s, kind:%d",hashTable[i]->name, hashTable[i]->type->kind);
			if(hashTable[i]->type->kind == BASIC) {
				if(hashTable[i]->type->u.basic == INT_TYPE)
					printf(" INT\n");
				else
					printf(" FLOAT\n");
			}
			else if(hashTable[i]->type->kind == FUNCTION) {
				if(hashTable[i]->type->u.function.retType->kind == BASIC) {
					if(hashTable[i]->type->u.function.retType->u.basic)
						printf(" return FLOAT\n");
					else
						printf(" return INT\n");
				}
				else if(hashTable[i]->type->u.function.retType->kind == STRUCTURE) {
					printf(" return %d\n", hashTable[i]->type->u.function.retType->kind);
				}
			}
			else if(hashTable[i]->type->kind == ARRAY) {
				if(hashTable[i]->type->u.array.elem->u.basic)
					printf(" is FLOAT ARRAY\n");
				else
					printf(" is INT ARRAY\n");
			}
			else
				printf("\n");
		}
	}
}


int isSameType(Type type1, Type type2) {
	if(!type1||!type2)//one is null
		return 0;
	if(type1->kind != type2->kind)
		return 0;
	if(type1->kind == BASIC) {
		if(type1->u.basic == type2->u.basic)
			return 1;
		else 
			return 0;
	}
	else if(type1->kind == ARRAY) {
		if(isSameType(type1->u.array.elem, type2->u.array.elem))
			return 1;
		else
			return 0;
	}
	else if(type1->kind == STRUCTURE) {
		FieldList s1 = type1->u.structure, s2 = type2->u.structure;
		return isSameFieldList(s1, s2);
	}
	else if(type1->kind == FUNCTION) {
		if(!isSameType(type1->u.function.retType, type2->u.function.retType))
			return 0;//return type differ
		if(type1->u.function.paraNum != type1->u.function.paraNum)
			return 0;//number of parameters differ
		FieldList p1 = type1->u.function.parameters, p2 = type2->u.function.parameters;
		return isSameFieldList(p1, p2);
	}
	return 0;
}

int isSameFieldList(FieldList f1, FieldList f2) {
	if(!f1||!f2)//one is null
		return 0;
	while(f1&&f2) {//compare components
		if(!isSameType(f1->type, f2->type))
			return 0;
		f1 = f1->tail;//s1 to next field
		f2 = f2->tail;//s2 to next field
	}
	if(f1||f2)
		return 0;//one end but another not
	else
		return 1;
}


void traverseTree(Node *root) {
    if(!strcmp(root->name, "Program"))
        Program(root);
}


void Program(Node *root) {
	//printf("Program\n");
	ExtDefList(root->FirstChild);
}

void ExtDefList(Node *root) {
	//printf("ExtDefList\n");
	Node *child = root->FirstChild;
	if(child) {
		ExtDef(child);
		ExtDefList(child->NextSib);
	}
}

void ExtDef(Node *root) {
	//printf("ExtDef\n");
	Node *child = root->FirstChild;//child point to Specifier
	Type type = Specifier(child);
	child = child->NextSib;//child point to ExtDecList/SEMI/FunDec
	if(!strcmp(child->name, "SEMI")) //Specifier SEMI
		return;
	if(!strcmp(child->name, "ExtDecList"))//Specifier ExtDecList SEMI
		ExtDecList(child, type);
	if(!strcmp(child->name, "FunDec")) { //Specifier FunDec CompSt
		FieldList f = FunDec(child, type);
		
		if(!searchTable(f->name, 1))
			insertTable(f);
		else
			printf("Error type 4 at Line %d: Redefined function '%s'\n", child->line_no, f->name);
		Operand Function = calloc(1, sizeof(struct Operand_));
		Function->kind = FUNCTION_OP;
		Function->u.value = malloc(100);
		strcpy(Function->u.value, f->name);
		InterCode functionIR = calloc(1, sizeof(struct InterCode_));
		functionIR->kind = FUNCTION_IR;
		functionIR->u.unaryOP.op = Function;
		insertCode(functionIR);
		f = f->type->u.function.parameters;
		while(f) {
			Operand Param = calloc(1, sizeof(struct Operand_));
			Param->kind = VARIABLE_OP;
			Param->u.value = malloc(100);
			strcpy(Param->u.value, f->name);
			InterCode paramIR = calloc(1, sizeof(struct InterCode_));
			paramIR->kind = PARAM_IR;
			paramIR->u.unaryOP.op = Param;
			insertCode(paramIR);
			f = f->tail;
		}
		child = child->NextSib;//child point to CompSt
		if(!strcmp(child->name, "CompSt"))
			CompSt(child, type);
	}
}

void ExtDecList(Node *root, Type type) {
	//printf("ExtDecList\n");
	Node *child = root->FirstChild;//child point to VarDec
	FieldList t = VarDec(child, type, 0);	
	child = child->NextSib;//child point to COMMA or NULL
	if(child) { //VarDec COMMA ExtDecList
		child = child->NextSib;//child point to ExtDecList
		ExtDecList(child, type);
	}
	if(t&&t->type->kind == ARRAY) {
		Operand Dec = calloc(1, sizeof(struct Operand_));
		Dec->kind = TEMP_VAR_OP;
		Dec->u.var_no = TempVarCount++;
		InterCode decIR = calloc(1, sizeof(struct InterCode_));
		decIR->kind = DEC_IR;
		decIR->u.decOP.op = Dec;
		decIR->u.decOP.size = getSize(t->type);
		insertCode(decIR);
		
		Operand ArrayDddr = calloc(1, sizeof(struct Operand_));
		ArrayDddr->kind = VARIABLE_OP;
		ArrayDddr->u.value = malloc(100);
		strcpy(ArrayDddr->u.value, t->name);
		InterCode addrIR = calloc(1, sizeof(struct InterCode_));
		addrIR->kind = GET_ADDR_IR;
		addrIR->u.binaryOP.left = ArrayDddr;
		addrIR->u.binaryOP.right = Dec;
		insertCode(addrIR);
	}
}


Type Specifier(Node *root) {
	//printf("Specifier\n");
	Node *child = root->FirstChild;
	Type type = malloc(sizeof(struct Type_));
	if(!strcmp(child->name, "TYPE")) {//int or float
		type->kind = BASIC;
		if(!strcmp(child->u.value_s, "int"))
			type->u.basic = INT_TYPE;
		else if(!strcmp(child->u.value_s, "float"))
			type->u.basic = FLOAT_TYPE;
	}
	else if(!strcmp(child->name, "StructSpecifier"))
		type = StructSpecifier(child);
	return type;
}

Type StructSpecifier(Node *root) {
	//printf("StructSpecifier\n");
	//printf("Cannot translate: Code contains variables or parameters of structure type\n");
	//exit(-1);
	Node *child = root->FirstChild;//point to STRUCT
	Type type = malloc(sizeof(struct Type_));
	type->kind = STRUCTURE;
	child = child->NextSib;//point to OptTag or Tag
	if(!strcmp(child->name, "Tag")) { //STRUCT Tag
		FieldList ret = searchTable(child->FirstChild->u.value_s, 0);
		if(!ret||ret->type->kind != STRUCTURE)
			printf("Error type 17 at Line %d: Undefined structure ‘%s’\n", child->line_no, child->FirstChild->u.value_s);
		if(ret)
			return ret->type;
		else {
			type->kind = STRUCTURE;
			type->u.structure = NULL;
			return type;
		}
	}
	else { //STRUCT OptTag LC DefList RC
		FieldList f = malloc(sizeof(struct FieldList_));
		f->tail = NULL;
		//child point to OptTag
		int OptTagLine = child->line_no;
		if(!child->FirstChild) {
			f->name = NULL;
		}
		else {
			f->name = malloc(100);
			strcpy(f->name, child->FirstChild->u.value_s);
		}
		child = child->NextSib;
		child = child->NextSib;
		//child point to DefList
		type->u.structure = DefList(child, 1);//isFromStruct=1
		f->type = type;
		if(!f->name)
			return type;//struct without name
		if(searchTable(f->name, 0))
			printf("Error type 16 at Line %d: Duplicated name ‘%s’\n", OptTagLine, f->name);
		else
			insertTable(f);
		return type;
	}
}


FieldList VarDec(Node *root, Type type, int isFromStruct) {
	//printf("VarDec\n");
	Node *child = root->FirstChild;//child point to ID or VarDec
	if(!strcmp(child->name, "ID")) { //ID
		FieldList f = malloc(sizeof(struct FieldList_));
		f->name = malloc(100);
		strcpy(f->name, child->u.value_s);
		f->type = type;
		f->tail = NULL;
		if(searchTable(f->name, 0)) {
			if(isFromStruct)
				printf("Error type 15 at line %d: Redefined field ‘%s’\n", child->line_no, f->name);
			else
				printf("Error type 3 at Line %d: Redefined variable ‘%s’\n", child->line_no, f->name);
		}
		else
			insertTable(f);
		return f;
	}
	else { //VarDec LB INT RB 
		FieldList f = VarDec(child, type, isFromStruct);//'f' is shared during iteration
		if(!f)
			return NULL;
		Type head = f->type;
		child = child->NextSib;
		child = child->NextSib;//child point to INT
		Type cur = malloc(sizeof(struct Type_));
		cur->kind = ARRAY;
		cur->u.array.size = child->u.value_i;
		cur->u.array.elem = type;
		if(head->kind != ARRAY) { //first dimension
			f->type = cur;
			return f;
		}
		while(head->u.array.elem->kind == ARRAY)
			head = head->u.array.elem; //multidimensional array
		head->u.array.elem = cur;
		return f;
	}
}

FieldList FunDec(Node *root, Type type) {
	//printf("FunDec\n");
	Node *child = root->FirstChild;//child point to ID
	FieldList f = malloc(sizeof(struct FieldList_));
	f->type = malloc(sizeof(struct Type_));
	f->name = malloc(100);
	strcpy(f->name, child->u.value_s);
	f->tail = NULL;
	f->type->kind = FUNCTION;
	f->type->u.function.retType = type;
	child = child->NextSib;//child point to LP
	child = child->NextSib;//child point to RP/VarList
	if(!strcmp(child->name, "VarList")) { //ID LP VarList RP
		f->type->u.function.parameters = VarList(child);
		f->type->u.function.paraNum = countParaNum(f->type->u.function.parameters);
	}
	else { //ID LP RP
		f->type->u.function.paraNum = 0;
		f->type->u.function.parameters = NULL;
	}
	return f;
}

int countParaNum(FieldList parameters) {
	int ret = 0;
	while(parameters) {
		ret++;
		parameters = parameters->tail;
	}
	return ret;
}

FieldList VarList(Node *root) {
	//printf("VarList\n");
	Node *child = root->FirstChild;//child point to ParamDec
	FieldList f = ParamDec(child);
	child = child->NextSib;//child point to COMMA or NULL
	if(child) {
		child = child->NextSib;//child point to VarList
		f->tail = VarList(child);
	}
	else 
		f->tail = NULL;
	return f;
}

FieldList ParamDec(Node *root) {
	//printf("ParamDec\n");
	Node *child = root->FirstChild;//child point to Specifier
	Type type = Specifier(child);
	child = child->NextSib;//child point to VarDec
	FieldList f = VarDec(child, type, 0);
	return f;
}


void CompSt(Node *root, Type type) {
	//printf("CompSt\n");
	Node *child = root->FirstChild;//child point to LC
	child = child->NextSib;//child point to DefList
	DefList(child, 0);
	child = child->NextSib;//child point to StmtList
	StmtList(child, type);
}

void StmtList(Node *root, Type type) {
	//printf("StmtList\n");
	Node *child = root->FirstChild;//child point to Stmt
	if(child) {
		Stmt(child, type);
		StmtList(child->NextSib, type);
	}
}

void Stmt(Node *root, Type type) {
	//printf("Stmt\n");
	Node *child = root->FirstChild;//child point to Exp/CompSt/RETURN/IF/WHILE
	if(!strcmp(child->name, "RETURN")) { //RETURN Exp SEMI
		child = child->NextSib;//child point to Exp
		Operand Return = calloc(1, sizeof(struct Operand_));
		if(!strcmp(child->FirstChild->name, "INT")) {
			Return->kind = CONSTANT_OP;
			Return->u.value = malloc(100);
			sprintf(Return->u.value, "%d", child->FirstChild->u.value_i);
		}
		else 
			Exp(child, Return);
		InterCode returnIR = calloc(1, sizeof(struct InterCode_));
		returnIR->kind = RETURN_IR;
		returnIR->u.unaryOP.op = Return;
		insertCode(returnIR);
		/*Type t = Exp(child);
		if(!isSameType(type, t)) {
			printf("Error type 8 at Line %d: Type mismatched for return\n", child->line_no);
			return;
		}*/
	}
	else if(!strcmp(child->name, "Exp")) //Exp SEMI
		Exp(child, NULL);
	else if(!strcmp(child->name, "CompSt")) //CompSt
		CompSt(child, type);
	else if(!strcmp(child->name, "WHILE")) { //WHILE LP EXP RP Stmt
		child = child->NextSib;//child point to LP
		child = child->NextSib;//child point to EXP
		Operand label1 = calloc(1, sizeof(struct Operand_));
		label1->kind = LABEL_OP;
		label1->u.label_no = LabelCount++;
		Operand label2 = calloc(1, sizeof(struct Operand_));
		label2->kind = LABEL_OP;
		label2->u.label_no = LabelCount++;
		Operand label3 = calloc(1, sizeof(struct Operand_));
		label3->kind = LABEL_OP;
		label3->u.label_no = LabelCount++;
		InterCode IR1 = calloc(1, sizeof(struct InterCode_));
		IR1->kind = LABEL_IR;
		IR1->u.unaryOP.op = label1;
		insertCode(IR1);
		Type type = Cond(child, label2, label3);
		InterCode IR2 = calloc(1, sizeof(struct InterCode_));
		IR2->kind = LABEL_IR;
		IR2->u.unaryOP.op = label2;
		insertCode(IR2);
		/*Type t = Exp(child);
        if(!((t->kind == BASIC)&&(t->u.basic == INT_TYPE)))
			printf("Error type 7 at Line %d: Type mismatched for operands\n", child->line_no);*/
        child = child->NextSib;//child point to RP
		child = child->NextSib;//child point to Stmt
		Stmt(child, type);
		InterCode gotoIR = calloc(1, sizeof(struct InterCode_));
		gotoIR->kind = GOTO_IR;
		gotoIR->u.unaryOP.op = label1;
		insertCode(gotoIR);
		InterCode IR3 = calloc(1, sizeof(struct InterCode_));
		IR3->kind = LABEL_IR;
		IR3->u.unaryOP.op = label3;
		insertCode(IR3);
	}
	else if(!strcmp(child->name, "IF")) { //IF LP EXP RP Stmt( ELSE Stmt)?
		child = child->NextSib;//child point to LP
		child = child->NextSib;//child point to EXP
		Operand label1 = calloc(1, sizeof(struct Operand_));
		label1->kind = LABEL_OP;
		label1->u.label_no = LabelCount++;
		Operand label2 = calloc(1, sizeof(struct Operand_));
		label2->kind = LABEL_OP;
		label2->u.label_no = LabelCount++;
		Type type = Cond(child, label1, label2);
		InterCode IR1 = calloc(1, sizeof(struct InterCode_));
		IR1->kind = LABEL_IR;
		IR1->u.unaryOP.op = label1;
		insertCode(IR1);
		/*Type t = Exp(child);
        if(!((t->kind == BASIC)&&(t->u.basic == INT_TYPE)))
            printf("Error type 7 at Line %d: Type mismatched for operands\n", child->line_no);*/
        child = child->NextSib;//child point to RP
		child = child->NextSib;//child point to Stmt
		Stmt(child, type);
		child = child->NextSib;//child point to ELSE or NULL
		if(child) { //IF LP EXP RP Stmt ELSE Stmt
			Operand label3 = calloc(1, sizeof(struct Operand_));
			label3->kind = LABEL_OP;
			label3->u.label_no = LabelCount++;
			InterCode gotoIR = calloc(1, sizeof(struct InterCode_));
			gotoIR->kind = GOTO_IR;
			gotoIR->u.unaryOP.op = label3;
			insertCode(gotoIR);			
			InterCode IR2 = calloc(1, sizeof(struct InterCode_));
			IR2->kind = LABEL_IR;
			IR2->u.unaryOP.op = label2;
			insertCode(IR2);
			Stmt(child->NextSib, type);
			InterCode IR3 = calloc(1, sizeof(struct InterCode_));
			IR3->kind = LABEL_IR;
			IR3->u.unaryOP.op = label3;
			insertCode(IR3);
		}
		else {
			InterCode IR2 = calloc(1, sizeof(struct InterCode_));
			IR2->kind = LABEL_IR;
			IR2->u.unaryOP.op = label2;
			insertCode(IR2);
		}
	}
}


FieldList DefList(Node *root, int isFromStruct) {
	//printf("DefList\n");
	Node *child = root->FirstChild;//child point to Def
	if(!child)
		return NULL;
	//Def DefList
	FieldList f = Def(child, isFromStruct);
	child = child->NextSib;//child point to DefList
	f->tail = DefList(child, isFromStruct);
	return f;
}

FieldList Def(Node *root, int isFromStruct) { //Specifier DecList SEMI
	//printf("Def\n");
	Node* child = root->FirstChild;//child point to Specifier
	FieldList f;
	Type type = Specifier(child);
	child = child->NextSib;//child potin to DecList
	f = DecList(child, type, isFromStruct);	//inhre
	return f;
}

FieldList DecList(Node *root, Type type, int isFromStruct) {
	//printf("DecList\n");
	Node *child = root->FirstChild;//child point to Dec
	FieldList f = Dec(child, type, isFromStruct);
	child = child->NextSib;//child point to COMMA or NULL
	if(child) {
		child = child->NextSib;//child point to DecList
		f->tail = DecList(child, type, isFromStruct);
	}
	else 
		f->tail = NULL;
	return f;
}

FieldList Dec(Node *root, Type type, int isFromStruct) {
	//printf("Dec\n");
	Node *child = root->FirstChild;//child point to VarDec
	FieldList f = VarDec(child, type, isFromStruct);
	if(f->type->kind == ARRAY) {
		Operand Dec = calloc(1, sizeof(struct Operand_));
		Dec->kind = TEMP_VAR_OP;
		Dec->u.var_no = TempVarCount++;
		InterCode decIR = calloc(1, sizeof(struct InterCode_));
		decIR->kind = DEC_IR;
		decIR->u.decOP.op = Dec;
		decIR->u.decOP.size = getSize(f->type);
		insertCode(decIR);
		
		Operand ArrayDddr = calloc(1, sizeof(struct Operand_));
		ArrayDddr->kind = VARIABLE_OP;
		ArrayDddr->u.value = malloc(100);
		strcpy(ArrayDddr->u.value, f->name);
		InterCode addrIR = calloc(1, sizeof(struct InterCode_));
		addrIR->kind = GET_ADDR_IR;
		addrIR->u.binaryOP.left = ArrayDddr;
		addrIR->u.binaryOP.right = Dec;
		insertCode(addrIR);
	}
	child = child->NextSib;//child point to ASSIGNOP or NULL
	if(child) { 
		//VarDec ASSIGNOP Exp
		if(isFromStruct) { 
			printf("Error type 15 at Line %d: Illegally initialized field ‘%s’\n", child->line_no, f->name);
			return f;
		}
		child = child->NextSib;//child point to Exp
		Operand place = calloc(1, sizeof(struct Operand_));
		place->kind = VARIABLE_OP;
		place->u.value = malloc(100);
		strcpy(place->u.value, f->name);
		char *prePlaceValue = malloc(100);
		strcpy(prePlaceValue, place->u.value);
		Type exp_type = Exp(child, place);
		if(strcmp(prePlaceValue, place->u.value)||place->kind != VARIABLE_OP) {
			Operand leftOp = calloc(1, sizeof(struct Operand_));
			leftOp->kind = VARIABLE_OP;
			leftOp->u.value = malloc(100);
			strcpy(leftOp->u.value, f->name);
			InterCode assignIR = calloc(1, sizeof(struct InterCode_));
			assignIR->kind = ASSIGN_IR;
			assignIR->u.binaryOP.left = leftOp;
			assignIR->u.binaryOP.right = place;
			insertCode(assignIR);
		}
		/*if(exp_type&&type&&!isSameType(type, exp_type))
			printf("Error type 5 at Line %d: Type mismatched for assignment\n", child->line_no);*/
		
	}
	return f;
}


Type Exp(Node *root, Operand place) {
	//printf("Exp\n");
	if(!root)
		return NULL;
	Node *child = root->FirstChild;//child point to the first
	if(!strcmp(child->name, "INT")) { //INT
		Type t = malloc(sizeof(struct Type_));
		t->kind = BASIC;
		t->u.basic = INT_TYPE;
		if(place) {
			/*Operand Int = calloc(1, sizeof(struct Operand_));
			Int->kind = CONSTANT_OP;
			Int->u.value = malloc(100);
			sprintf(Int->u.value, "%d", child->u.value_i);
			InterCode intIR = calloc(1, sizeof(struct InterCode_));
			intIR->kind = ASSIGN_IR;
			intIR->u.binaryOP.right = Int;
			place->kind = TEMP_VAR_OP;
			place->u.var_no = TempVarCount++;
			intIR->u.binaryOP.left = place;
			insertCode(intIR);*/
			place->kind = CONSTANT_OP;
			place->u.value = malloc(100);
			sprintf(place->u.value, "%d", child->u.value_i);
		}
		return t;
	}
	else if(!strcmp(child->name, "FLOAT")) { //FLOAT
		Type t = malloc(sizeof(struct Type_));
		t->kind = BASIC;
		t->u.basic = FLOAT_TYPE;
		return t;
	}
	else if(!strcmp(child->name, "ID")&&!child->NextSib) { //ID
		FieldList f = searchTable(child->u.value_s, 0);
		if(!f) {
			printf("Error type 1 at Line %d: Undefined variable '%s'\n", child->line_no, child->u.value_s);	
			return NULL;
		}
		if(place) {
			place->kind = VARIABLE_OP;
			place->u.value = malloc(100);
			strcpy(place->u.value, child->u.value_s);
		}
		return f->type;
	}
	else if(!strcmp(child->name, "Exp")) { //start with Exp
		Node *left = child;
		Type leftType, rightType;
		child = child->NextSib;//child point to =/||/&&/RELOP/+/-/*/DIV/[/.
		if(!strcmp(child->name, "ASSIGNOP")) { //Exp ASSIGNOP Exp
			Node *right = child->NextSib;
			/*InterCode assignIR1 = calloc(1, sizeof(struct InterCode_));
			assignIR1->kind = ASSIGN_IR;
			Operand temp1 = calloc(1, sizeof(struct Operand_));
			temp1->kind = TEMP_VAR_OP;
			leftType = Exp(left, temp1);
			Operand temp2 = calloc(1, sizeof(struct Operand_));
			if(!strcmp(right->FirstChild->name, "INT")) {
				rightType = Exp(right, NULL);
				temp2->kind = CONSTANT_OP;
				temp2->u.value = malloc(100);
				sprintf(temp2->u.value, "%d", right->FirstChild->u.value_i);
			}
			else {
				rightType = Exp(right, temp2);
				temp2->kind = TEMP_VAR_OP;
			}
			assignIR1->u.binaryOP.left = temp1;
			assignIR1->u.binaryOP.right = temp2;
			insertCode(assignIR1);
			if(place) {
				InterCode assignIR2 = calloc(1, sizeof(struct InterCode_));
				assignIR2->kind = ASSIGN_IR;
				assignIR2->u.binaryOP.left = place;
				assignIR2->u.binaryOP.right = temp2;
				insertCode(assignIR2);
			}*/
			Operand temp1 = calloc(1, sizeof(struct Operand_));
			temp1->kind = TEMP_VAR_OP;
			temp1->u.var_no = TempVarCount++;
			leftType = Exp(left, temp1);
			Operand temp2 = calloc(1, sizeof(struct Operand_));
			temp2->kind = TEMP_VAR_OP;
			temp2->u.var_no = TempVarCount++;
			int temp2no = temp2->u.var_no;
			rightType = Exp(right, temp2);
			if(!(temp2->kind == TEMP_VAR_OP&&temp2->u.var_no == temp2no&&(temp1->kind == TEMP_VAR_OP||temp1->kind == VARIABLE_OP))){
				//temp2 changed
				InterCode assignIR1 = calloc(1, sizeof(struct InterCode_));
				assignIR1->kind = ASSIGN_IR;
				assignIR1->u.binaryOP.left = temp1;
				assignIR1->u.binaryOP.right = temp2;
				insertCode(assignIR1);
			}
			else{
				memcpy(temp2, temp1, sizeof(struct Operand_));
			}
			if(place) {
				InterCode assignIR2 = calloc(1, sizeof(struct InterCode_));
				assignIR2->kind = ASSIGN_IR;
				assignIR2->u.binaryOP.left = place;
				assignIR2->u.binaryOP.right = temp2;
				insertCode(assignIR2);
			}
			if(!strcmp(left->FirstChild->name, "ID")&&!left->FirstChild->NextSib)//left is ID
				;//leftType = Exp(left);
			else if(!strcmp(left->FirstChild->name, "Exp")&&left->FirstChild->NextSib&&!strcmp(left->FirstChild->NextSib->name, "LB"))//left is Exp LB Exp RB
				;//leftType = Exp(left);
			else if(!strcmp(left->FirstChild->name, "Exp")&&left->FirstChild->NextSib&&!strcmp(left->FirstChild->NextSib->name, "DOT"))//left is Exp DOT ID
				;//leftType = Exp(left);
			else
				printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable\n", left->line_no);
			/*rightType = Exp(right);
			if(isSameType(leftType, rightType))
				return leftType;
			else if(leftType) {
				printf("Error type 5 at Line %d: Type mismatched for assignment\n", child->line_no);
				return NULL;
			}
			return NULL;*/
			return leftType;
		}
		else if(!strcmp(child->name, "AND")||!strcmp(child->name, "OR")) { //Exp AND Exp / Exp OR Exp
			//Node *right = child->NextSib;
			//label1
			Operand label1 = calloc(1, sizeof(struct Operand_));
			label1->kind = LABEL_OP;
			label1->u.label_no = LabelCount++;
			//lavbel2
			Operand label2 = calloc(1, sizeof(struct Operand_));
			label2->kind = LABEL_OP;
			label2->u.label_no = LabelCount++;
			//code0
			if(place) {
				InterCode code0 = calloc(1, sizeof(struct InterCode_));
				code0->kind = ASSIGN_IR;
				code0->u.binaryOP.left = place;
				Operand zero = calloc(1, sizeof(struct Operand_));
				zero->kind = CONSTANT_OP;
				zero->u.value = malloc(5);
				strcpy(zero->u.value, "0");
				code0->u.binaryOP.right = zero;
				insertCode(code0);
			}
			/*leftType = Exp(left, NULL);
			rightType = Exp(right, NULL);
			Type ret = malloc(sizeof(struct Type_));
			ret->kind = BASIC;
			ret->u.basic = INT_TYPE;*/
			//code1
			Type code1 = Cond(root, label1, label2);
			//code2
			InterCode temp1 = calloc(1, sizeof(struct InterCode_));
			temp1->kind = LABEL_IR;
			temp1->u.unaryOP.op = label1;
			insertCode(temp1);
			if(place) {
				InterCode code2 = calloc(1, sizeof(struct InterCode_));
				code2->kind = ASSIGN_IR;
				code2->u.binaryOP.left = place;
				Operand one = calloc(1, sizeof(struct Operand_));
				one->kind = CONSTANT_OP;
				one->u.value = malloc(5);
				strcpy(one->u.value, "1");
				code2->u.binaryOP.right = one;
				insertCode(code2);
			}
			//label2
			InterCode temp2 = calloc(1, sizeof(struct InterCode_));
			temp2->kind = LABEL_IR;
			temp2->u.unaryOP.op = label2;
			insertCode(temp2);
			
			/*if(isSameType(leftType, ret)&&isSameType(rightType, ret))
				return ret;
			else {
				printf("Error type 7 at Line %d: Type mismatched for operands\n", child->line_no);
				return NULL;
			}*/
			return code1;
		}
		else if(!strcmp(child->name, "PLUS")||!strcmp(child->name, "MINUS")||!strcmp(child->name, "STAR")||!strcmp(child->name, "DIV")) { //+-*/
			Node *right = child->NextSib;
			Operand temp1 = calloc(1, sizeof(struct Operand_));
			temp1->kind = TEMP_VAR_OP;
			temp1->u.var_no = TempVarCount++;
			leftType = Exp(left, temp1);
			Operand temp2 = calloc(1, sizeof(struct Operand_));
			temp2->kind = TEMP_VAR_OP;
			temp2->u.var_no = TempVarCount++;
			rightType = Exp(right, temp2);
			/*if(isSameType(leftType, rightType))
				return leftType;
			else if(!leftType||!rightType) {
				;
			}
			else {
				printf("Error type 7 at Line %d: Type mismatched for operands\n", child->line_no);
			}*/
			if(!place) {
				//printf("1\n");
				return leftType;
			}
			else {
				InterCode computeIR = calloc(1, sizeof(struct InterCode_));
				if(!strcmp(child->name, "PLUS"))
					computeIR->kind = PLUS_IR;
				else if(!strcmp(child->name, "MINUS"))
					computeIR->kind = SUB_IR;
				else if(!strcmp(child->name, "STAR"))
					computeIR->kind = MUL_IR;
				else if(!strcmp(child->name, "DIV"))
					computeIR->kind = DIV_IR;
				
				//place->kind = TEMP_VAR_OP;
				//place->u.var_no = TempVarCount++;
				computeIR->u.ternaryOP.op1 = temp1;
				computeIR->u.ternaryOP.op2 = temp2;
				computeIR->u.ternaryOP.result = place;
				insertCode(computeIR);
			}
			if(leftType->kind == BASIC)
				return leftType;
			else
				return rightType;
		}
		else if(!strcmp(child->name, "RELOP")) { //Exp RELOP Exp
			//Node *right = child->NextSib;
			//label1
			Operand label1 = calloc(1, sizeof(struct Operand_));
			label1->kind = LABEL_OP;
			label1->u.label_no = LabelCount++;
			//lavbel2
			Operand label2 = calloc(1, sizeof(struct Operand_));
			label2->kind = LABEL_OP;
			label2->u.label_no = LabelCount++;
			//code0
			if(place) {
				InterCode code0 = calloc(1, sizeof(struct InterCode_));
				code0->kind = ASSIGN_IR;
				code0->u.binaryOP.left = place;
				Operand zero = calloc(1, sizeof(struct Operand_));
				zero->kind = CONSTANT_OP;
				zero->u.value = malloc(5);
				strcpy(zero->u.value, "0");
				code0->u.binaryOP.right = zero;
				insertCode(code0);
			}
			/*leftType = Exp(left, NULL);
			rightType = Exp(right, NULL);
			if(isSameType(leftType, rightType)) {
				;
			}
			else if(!leftType||!rightType) {
				;
			}
			else {
				printf("Error type 7 at Line %d: Type mismatched for operands\n", child->line_no);
			}
			Type ret = malloc(sizeof(struct Type_));
			ret->kind = BASIC;
			ret->u.basic = INT_TYPE;*/
			//code1
			Type code1 = Cond(root, label1, label2);
			//code2
			InterCode temp1 = calloc(1, sizeof(struct InterCode_));
			temp1->kind = LABEL_IR;
			temp1->u.unaryOP.op = label1;
			insertCode(temp1);
			if(place) {
				InterCode code2 = calloc(1, sizeof(struct InterCode_));
				code2->kind = ASSIGN_IR;
				code2->u.binaryOP.left = place;
				Operand one = calloc(1, sizeof(struct Operand_));
				one->kind = CONSTANT_OP;
				one->u.value = malloc(5);
				strcpy(one->u.value, "1");
				code2->u.binaryOP.right = one;
				insertCode(code2);
			}
			//label2
			InterCode temp2 = calloc(1, sizeof(struct InterCode_));
			temp2->kind = LABEL_IR;
			temp2->u.unaryOP.op = label2;
			insertCode(temp2);
			//return ret;
			return code1;
		}
		else if(!strcmp(child->name, "LB")) { //Exp LB Exp RB
			Operand baseOp = calloc(1, sizeof(struct Operand_));
			baseOp->kind = TEMP_VAR_OP;
			baseOp->u.var_no = TempVarCount++;
			leftType = Exp(left, baseOp);
			int index = 1;
			if(!strcmp(left->NextSib->NextSib->FirstChild->name, "INT")) {
				index = left->NextSib->NextSib->FirstChild->u.value_i;
				//printf("%d\n",index);
			}
			Operand indexOp = calloc(1, sizeof(struct Operand_));
			if(index) {
				indexOp->kind = TEMP_VAR_OP;
				indexOp->u.var_no = TempVarCount++;
			}			
			Node *right = child->NextSib;
			rightType = Exp(right, indexOp);
			Operand offsetOp = calloc(1, sizeof(struct Operand_));
			offsetOp->kind = TEMP_VAR_OP;
			offsetOp->u.var_no = TempVarCount++;
			if(index) {
				//example a[i][j][k] compute sizeof [j][k]
				Operand numOp = calloc(1, sizeof(struct Operand_));
				numOp->kind = CONSTANT_OP;
				numOp->u.value = malloc(5);
				sprintf(numOp->u.value, "%d", getSize(leftType->u.array.elem));
				InterCode offsetIR = calloc(1, sizeof(struct InterCode_));
				offsetIR->kind = MUL_IR;
				offsetIR->u.ternaryOP.result = offsetOp;
				offsetIR->u.ternaryOP.op1 = numOp;
				offsetIR->u.ternaryOP.op2 = indexOp;
				insertCode(offsetIR);
				InterCode addrIR = calloc(1, sizeof(struct InterCode_));
				addrIR->kind = PLUS_IR;
				addrIR->u.ternaryOP.op1 = baseOp;
				addrIR->u.ternaryOP.op2 = offsetOp;
				if(leftType->u.array.elem->kind == BASIC) {
					//next is basic
					Operand tempOp = calloc(1, sizeof(struct Operand_));
					tempOp->kind = TEMP_VAR_OP;
					tempOp->u.var_no = TempVarCount++;
					addrIR->u.ternaryOP.result = tempOp;
					place->kind = ADDRESS_OP;
					place->u.name = tempOp;
				}
				else {
					//next is array
					addrIR->u.ternaryOP.result = place;
				}
				insertCode(addrIR);
			}
			else {
				InterCode addrIR = calloc(1, sizeof(struct InterCode_));
				addrIR->kind = ASSIGN_IR;
				addrIR->u.binaryOP.right = baseOp;
				if(leftType->u.array.elem->kind == BASIC) {
					//next is basic
					Operand tempOp = calloc(1, sizeof(struct Operand_));
					tempOp->kind = TEMP_VAR_OP;
					tempOp->u.var_no = TempVarCount++;
					addrIR->u.binaryOP.left = tempOp;
					place->kind = ADDRESS_OP;
					place->u.name = tempOp;
				}
				else {
					//next is array
					addrIR->u.binaryOP.right = place;
				}
				insertCode(addrIR);
			}
			/*if(!leftType)
				return NULL;
			if(leftType->kind != ARRAY) {
				printf("Error type 10 at Line %d: The variable is not an array\n", child->line_no);
				return NULL;
			}
			if(!rightType)
				return leftType->u.array.elem;
			if((rightType->kind != BASIC)||(rightType->u.basic == FLOAT_TYPE)) {
				printf("Error type 12 at Line %d: The index is not an integer\n", child->line_no);
				return leftType->u.array.elem;
			}*/
			return leftType->u.array.elem;
		}
		else if(!strcmp(child->name, "DOT")) { //Exp DOT ID
			Node *right = child->NextSib;
			leftType = Exp(left, NULL);
			if(!leftType)
				return NULL;
			if(leftType->kind != STRUCTURE) {
				printf("Error type 13 at Line %d: Illegal use of '.'\n", child->line_no);
				return NULL;
			}
			FieldList f = leftType->u.structure;
			while(f) {
				if(!strcmp(f->name, right->u.value_s))
					return f->type;
				f = f->tail;
			}
			printf("Error type 14 at line %d: Non-existent field '%s'\n", right->line_no, right->u.value_s);
			return NULL;
		}
	}
	else if(!strcmp(child->name, "LP")) { //LP Exp RP
		child = child->NextSib;//child point to Exp
		return Exp(child, place);
	}
	else if(!strcmp(child->name, "MINUS")) { //MINUS Exp
		child = child->NextSib;//child point to Exp
		/*if(!strcmp(child->FirstChild->name, "INT")&&place) {
			int num = child->FirstChild->u.value_i;
			if(num >= 0) {
				Type ret = Exp(child, NULL);
				place->kind = CONSTANT_OP;
				place->u.value = malloc(100);
				sprintf(place->u.value, "-%d", num);
				return ret;
			}
		}*/
		Operand EXp = calloc(1, sizeof(struct Operand_));
		EXp->kind = TEMP_VAR_OP;
		EXp->u.var_no = TempVarCount++;
		Type ret = Exp(child, EXp);
		if(!ret)
			return NULL;
		if(ret->kind != BASIC) {
			printf("Error type 7 at Line %d: Type mismatched for operands\n", child->line_no);
			return NULL;
		}
		Operand zero = calloc(1, sizeof(struct Operand_));
		zero->kind = CONSTANT_OP;
		zero->u.value = malloc(5);
		strcpy(zero->u.value, "0");
		if(place) {
			InterCode minusIR = calloc(1, sizeof(struct InterCode_));
			minusIR->kind = SUB_IR;
			minusIR->u.ternaryOP.result = place;
			minusIR->u.ternaryOP.op1 = zero;
			minusIR->u.ternaryOP.op2 = EXp;
			insertCode(minusIR);
		}
		return ret;
	}
	else if(!strcmp(child->name, "NOT")) { //NOT Exp
		child = child->NextSib;//child point to Exp
		//label1
		Operand label1 = calloc(1, sizeof(struct Operand_));
		label1->kind = LABEL_OP;
		label1->u.label_no = LabelCount++;
		//lavbel2
		Operand label2 = calloc(1, sizeof(struct Operand_));
		label2->kind = LABEL_OP;
		label2->u.label_no = LabelCount++;
		//code0
		if(place) {
			InterCode code0 = calloc(1, sizeof(struct InterCode_));
			code0->kind = ASSIGN_IR;
			code0->u.binaryOP.left = place;
			Operand zero = calloc(1, sizeof(struct Operand_));
			zero->kind = CONSTANT_OP;
			zero->u.value = malloc(5);
			strcpy(zero->u.value, "0");
			code0->u.binaryOP.right = zero;
			insertCode(code0);
		}
		//Type ret = Exp(child, NULL);
		//code1
		Type code1 = Cond(root, label1, label2);
		//code2
		InterCode temp1 = calloc(1, sizeof(struct InterCode_));
		temp1->kind = LABEL_IR;
		temp1->u.unaryOP.op = label1;
		insertCode(temp1);
		if(place) {
			InterCode code2 = calloc(1, sizeof(struct InterCode_));
			code2->kind = ASSIGN_IR;
			code2->u.binaryOP.left = place;
			Operand one = calloc(1, sizeof(struct Operand_));
			one->kind = CONSTANT_OP;
			one->u.value = malloc(5);
			strcpy(one->u.value, "1");
			code2->u.binaryOP.right = one;
			insertCode(code2);
		}
		//label2
		InterCode temp2 = calloc(1, sizeof(struct InterCode_));
		temp2->kind = LABEL_IR;
		temp2->u.unaryOP.op = label2;
		insertCode(temp2);
		/*if(!ret)
			return NULL;
		if(ret->kind != BASIC||ret->u.basic == FLOAT_TYPE) {
			printf("Error type 7 at Line %d: Type mismatched for operands\n", child->line_no);
			return NULL;
		}
		return ret;*/
		return code1;
	}
	else if(!strcmp(child->name, "ID")) { //ID LP (Args )?RP
		FieldList f = searchTable(child->u.value_s, 1);
		if(!f) {
			FieldList not_f = searchTable(child->u.value_s, 0);//whether exists BASIC/ARRAY/STRUCTURE with same name
			if(not_f)
				printf("Error type 11 at Line %d: '%s' is not a function\n", child->line_no, child->u.value_s);
			else
				printf("Error type 2 at Line %d: Undefined function '%s'\n", child->line_no, child->u.value_s);
			return NULL;
		}
		Node *first = child;
		FieldList param = f->type->u.function.parameters;
		child = child->NextSib;//child point to LP
		child = child->NextSib;//child point to Args or RP
		if(!strcmp(child->name, "RP")) { //ID LP RP
			if(param)
				printf("Error type 9 at Line %d: The function '%s' is not applicable for arguments\n", child->line_no, f->name);
			if(!strcmp(first->u.value_s, "read")) {
				if(place) {
					InterCode readIR = calloc(1, sizeof(struct InterCode_));
					readIR->kind = READ_IR;
					place->kind = TEMP_VAR_OP;
					place->u.var_no = TempVarCount++;
					readIR->u.unaryOP.op = place;
					insertCode(readIR);
				}
			}
			else {
				Operand Function = calloc(1, sizeof(struct Operand_));
				Function->kind = FUNCTION_OP;
				Function->u.value = malloc(100);
				strcpy(Function->u.value, first->u.value_s);
				InterCode callIR = calloc(1, sizeof(struct InterCode_));
				callIR->kind = CALL_IR;
				if(place) {
					callIR->u.binaryOP.left = place;
				}
				else {
					Operand temp = calloc(1, sizeof(struct Operand_));
					temp->kind = TEMP_VAR_OP;
					temp->u.var_no = TempVarCount++;
					callIR->u.binaryOP.left = temp;
					//TODO
				}
				callIR->u.binaryOP.right = Function;
				insertCode(callIR);
			}
		}
		else { //ID LP Args RP
			/*FieldList a = Args(child);
			if(!isSameFieldList(param, a))
				printf("Error type 9 at Line %d: The function '%s' is not applicable for arguments\n", child->line_no, f->name);*/
			Operand Arglist = NULL;
			Args(child, &Arglist);
			if(!strcmp(first->u.value_s, "write")) {
				InterCode writeIR = calloc(1, sizeof(struct InterCode_));
				writeIR->kind = WRITE_IR;
				writeIR->u.unaryOP.op = Arglist;
				/*if(!Arglist)
					printf("1\n");*/
				insertCode(writeIR);
			}
			else {
				while(Arglist) {
					InterCode argIR = calloc(1, sizeof(struct InterCode_));
					argIR->kind = ARG_IR;
					argIR->u.unaryOP.op = Arglist;
					Arglist = Arglist->next;
					insertCode(argIR);
				}
				Operand Function = calloc(1, sizeof(struct Operand_));
				Function->kind = FUNCTION_OP;
				Function->u.value = malloc(100);
				strcpy(Function->u.value, first->u.value_s);
				InterCode callIR = calloc(1, sizeof(struct InterCode_));
				callIR->kind = CALL_IR;
				if(place) {
					callIR->u.binaryOP.left = place;
				}
				else {
					Operand temp = calloc(1, sizeof(struct Operand_));
					temp->kind = TEMP_VAR_OP;
					temp->u.var_no = TempVarCount++;
					callIR->u.binaryOP.left = temp;
				}
				callIR->u.binaryOP.right = Function;
				insertCode(callIR);
			}
		}
		return f->type->u.function.retType;
	}
}

int Args(Node *root, Operand *ArgList) {
	//printf("Args\n");
	/*Node *child = root->FirstChild;//child point to Exp
	FieldList f = malloc(sizeof(struct FieldList_));
	f->type = Exp(child);
	child = child->NextSib;//child point to COMMA or NULL
	if(child) {
		child = child->NextSib;//child point to Args
		f->tail = Args(child);
	}
	else
		f->tail = NULL;
	return f;*/
	Node *child = root->FirstChild;//child point to Exp
	Operand temp1 = calloc(1, sizeof(struct Operand_));
	temp1->kind = TEMP_VAR_OP;
	temp1->u.var_no = TempVarCount++;
	Type type = Exp(child, temp1);
	temp1->next = *ArgList;
	*ArgList = temp1;
	child = child->NextSib;//child point to COMMA or NULL
	if(child) {
		child = child->NextSib;//child point to Args
		return Args(child, ArgList);
	}
	else
		return 1;
}

void printFun(FieldList f) {
	/*printf("'%s(", f->name);
	FieldList param = f->type->u.function.parameters;
	if(!param) {//no parameters
		printf(")");
		return;
	}
	else 
		printf("%s", param->type.kind);*/
	
	;
}

void printArgs(FieldList f) {
	;
}

int getSize(Type type) {
	if(type->kind == BASIC)
		return 4;
	else if(type->kind == ARRAY) { 
		/*if(type->u.array.elem->kind == ARRAY) //multi-dimensional array
			return type->u.array.size * getSize(type->u.array.elem); 
		else //one dimensional array
			return type->u.array.size * 4; */
		return type->u.array.size * getSize(type->u.array.elem); 
	}
	else if(type->kind == STRUCTURE) {
		//printf("Cannot translate: Code contains variables or parameters of structure type\n");
		//exit(-1);
		;
	}
	return -1;
}

Type Cond(Node *root, Operand trueLabel, Operand falseLabel) {
	//printf("Cond\n");
	Node *child = root->FirstChild;//child point to the first
	if(!strcmp(child->name, "Exp")) {
		Node *left = child;
		Node *mid = child->NextSib;
		Node *right = mid->NextSib;
		if(!strcmp(mid->name, "RELOP")) { //EXP1 RELOP EXP2
			//code1
			Operand temp1 = calloc(1, sizeof(struct Operand_));
			Type type1;
			if(!strcmp(left->FirstChild->name, "INT")) {
				type1 = Exp(left, NULL);
				temp1->kind = CONSTANT_OP;
				temp1->u.value = malloc(100);
				sprintf(temp1->u.value, "%d", left->FirstChild->u.value_i);
			}
			else{
				temp1->kind = TEMP_VAR_OP;
				type1 = Exp(left, temp1);
			}
			//code2
			Operand temp2 = calloc(1, sizeof(struct Operand_));
			Type type2;
			if(!strcmp(right->FirstChild->name, "INT")) {
				type2 = Exp(right, NULL);
				temp2->kind = CONSTANT_OP;
				temp2->u.value = malloc(100);
				sprintf(temp2->u.value, "%d", right->FirstChild->u.value_i);
			}
			else {
				temp2->kind = TEMP_VAR_OP;
				type2 = Exp(right, temp2);
			}
			if(!type1||!type2)
				return NULL;			
			//code3 goto label_true
			InterCode ifgotoIR = calloc(1, sizeof(struct InterCode_));
			ifgotoIR->kind = IF_GOTO_IR;
			ifgotoIR->u.ifgotoOP.op1 = temp1;
			ifgotoIR->u.ifgotoOP.op2 = temp2;
			ifgotoIR->u.ifgotoOP.label = trueLabel;
			strcpy(ifgotoIR->u.ifgotoOP.relop, mid->u.value_s);
			insertCode(ifgotoIR);
			//code3 goto label_false
			InterCode gotoIR = calloc(1, sizeof(struct InterCode_));
			gotoIR->kind = GOTO_IR;
			gotoIR->u.unaryOP.op = falseLabel;
			insertCode(gotoIR);
			return type1;
		}
		else if(!strcmp(mid->name, "AND")) { //EXP1 AND EXP2
			Operand label1 = calloc(1, sizeof(struct Operand_));
			label1->kind = LABEL_OP;
			label1->u.label_no = LabelCount++;
			//code1
			Type type1 = Cond(left, label1, falseLabel);
			//label1
			InterCode labelIR = calloc(1, sizeof(struct InterCode_));
			labelIR->kind = LABEL_IR;
			labelIR->u.unaryOP.op = label1;
			insertCode(labelIR);
			//code2
			Type type2 = Cond(right, trueLabel, falseLabel);	
			return type1;
		}
		else if(!strcmp(mid->name, "OR")) { //EXP1 OR EXP2
			Operand label1 = calloc(1, sizeof(struct Operand_));
			label1->kind = LABEL_OP;
			label1->u.label_no = LabelCount++;
			//code1
			Type type1 = Cond(left, trueLabel, label1);
			//label1
			InterCode labelIR = calloc(1, sizeof(struct InterCode_));
			labelIR->kind = LABEL_IR;
			labelIR->u.unaryOP.op = label1;
			insertCode(labelIR);
			//code2
			Type type2 = Cond(right, trueLabel, falseLabel);	
			return type1;
		}
	}
	else if(!strcmp(child->name, "NOT")) { //NOT EXP1
		return Cond(child->NextSib, falseLabel, trueLabel);
	}
	else { //other cases
		//code1
		Operand temp = calloc(1, sizeof(Operand));
		Type type;
		if(!strcmp(child->name, "INT")) {
			type = Exp(root, NULL);
			temp->kind = CONSTANT_OP;
			temp->u.value = malloc(100);
			sprintf(temp->u.value, "%d", child->u.value_i);
		}
		else {
			temp->kind = TEMP_VAR_OP;
			type = Exp(root, temp);
		}
		//code2
		InterCode ifgotoIR = calloc(1, sizeof(struct InterCode_));
		ifgotoIR->kind = IF_GOTO_IR;
		ifgotoIR->u.ifgotoOP.op1 = temp;
		strcpy(ifgotoIR->u.ifgotoOP.relop, "!=");
		Operand zero = calloc(1, sizeof(struct Operand_));
		zero->kind = CONSTANT_OP;
		zero->u.value = malloc(5);
		strcpy(zero->u.value, "0");
		ifgotoIR->u.ifgotoOP.op2 = zero;
		ifgotoIR->u.ifgotoOP.label = trueLabel;
		insertCode(ifgotoIR);
		//return label_false
		InterCode gotoIR = calloc(1, sizeof(struct InterCode_));
		gotoIR->kind = GOTO_IR;
		gotoIR->u.unaryOP.op = falseLabel;
		insertCode(gotoIR);
		return type;
	}
	return NULL;
}
