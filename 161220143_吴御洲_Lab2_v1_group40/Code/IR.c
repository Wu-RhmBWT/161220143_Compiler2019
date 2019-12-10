#include "IR.h"

InterCode IRLIST_HEAD, IRLIST_TAIL;

int VarCount = 1, LabelCount = 1, TempVarCount = 1;

void initIRList() {
	IRLIST_HEAD = NULL;
	IRLIST_TAIL = NULL;
}

void insertCode(InterCode insert) {
	insert->next = NULL;
	insert->pre = NULL;
	if(!IRLIST_HEAD) { //list is empty
		IRLIST_HEAD = insert;
		IRLIST_TAIL = insert;
	}
	else {
		insert->pre = IRLIST_TAIL;
		IRLIST_TAIL->next = insert;
		IRLIST_TAIL = insert;
	}
}

void deleteCode(InterCode del) {
	if(del == IRLIST_HEAD&&del == IRLIST_TAIL) { //only one element
		IRLIST_HEAD = NULL;
		IRLIST_TAIL = NULL;
	}
	else if(del == IRLIST_HEAD) { //delete head
		del->next->pre = NULL;
		IRLIST_HEAD = IRLIST_HEAD->next;
	}
	else if(del == IRLIST_TAIL) { //delete tail
		del->pre->next = NULL;
		IRLIST_TAIL = IRLIST_TAIL->pre;
	}
	else {
		del->next->pre = del->pre;
		del->pre->next = del->next;
	}
}

void writeCode(char *filename) {
	FILE *fp = fopen(filename,"w");
	if(!fp) {
		printf("cannot open file %s\n", filename);
		return;
	}
	InterCode ir = IRLIST_HEAD;
	while(ir) {
		switch(ir->kind)
		{
			case LABEL_IR:
				fputs("LABEL ", fp);
				writeOp(ir->u.unaryOP.op, fp);
				fputs(" : ", fp);
				break;
			case FUNCTION_IR:
				fputs("FUNCTION ", fp);
				writeOp(ir->u.unaryOP.op, fp);
				fputs(" : ", fp);
				break;
			case ASSIGN_IR:
				writeOp(ir->u.binaryOP.left, fp);
				fputs(" := ", fp);
				writeOp(ir->u.binaryOP.right, fp);
				break;
			case PLUS_IR:
				writeOp(ir->u.ternaryOP.result, fp);
				fputs(" := ", fp);
				writeOp(ir->u.ternaryOP.op1, fp);
				fputs(" + ", fp);
				writeOp(ir->u.ternaryOP.op2, fp);
				break;
			case SUB_IR:
				writeOp(ir->u.ternaryOP.result, fp);
				fputs(" := ", fp);
				writeOp(ir->u.ternaryOP.op1, fp);
				fputs(" - ", fp);
				writeOp(ir->u.ternaryOP.op2, fp);
				break;
			case MUL_IR:
				writeOp(ir->u.ternaryOP.result, fp);
				fputs(" := ", fp);
				writeOp(ir->u.ternaryOP.op1, fp);
				fputs(" * ", fp);
				writeOp(ir->u.ternaryOP.op2, fp);
				break;
			case DIV_IR:
				writeOp(ir->u.ternaryOP.result, fp);
				fputs(" := ", fp);
				writeOp(ir->u.ternaryOP.op1, fp);
				fputs(" / ", fp);
				writeOp(ir->u.ternaryOP.op2, fp);
				break;
			case GET_ADDR_IR:
				writeOp(ir->u.binaryOP.left, fp);
				fputs(" := &", fp);
				writeOp(ir->u.binaryOP.right, fp);
				break;
			case GET_VALUE_IR:
				writeOp(ir->u.binaryOP.left, fp);
				fputs(" := *", fp);
				writeOp(ir->u.binaryOP.right, fp);
				break;
			case TO_MEMORY_IR:
				fputs("*", fp);
				writeOp(ir->u.binaryOP.left, fp);
				fputs(" := ", fp);
				writeOp(ir->u.binaryOP.right, fp);
				break;
			case GOTO_IR:
				fputs("GOTO ", fp);
				writeOp(ir->u.unaryOP.op, fp);
				break;
			case IF_GOTO_IR:
				fputs("IF ", fp);
				writeOp(ir->u.ifgotoOP.op1, fp);
				fputs(" ", fp);
				fputs(ir->u.ifgotoOP.relop, fp);
				fputs(" ", fp);
				writeOp(ir->u.ifgotoOP.op2, fp);
				fputs(" GOTO ", fp);
				writeOp(ir->u.ifgotoOP.label, fp);
				break;
			case RETURN_IR:
				fputs("RETURN ", fp);
				writeOp(ir->u.unaryOP.op, fp);
				break;
			case DEC_IR:
				fputs("DEC ", fp);
				writeOp(ir->u.decOP.op, fp);
				char str[32];
				sprintf(str, " %d ", ir->u.decOP.size);
				fputs(str, fp);
				break;
			case ARG_IR:
				fputs("ARG ", fp);
				writeOp(ir->u.unaryOP.op, fp);
				break;
			case CALL_IR:
				writeOp(ir->u.binaryOP.left, fp);
				fputs(" := CALL ", fp);
				writeOp(ir->u.binaryOP.right, fp);
				break;
			case PARAM_IR:
				fputs("PARAM ", fp);
				writeOp(ir->u.unaryOP.op, fp);
				break;
			case READ_IR:
				fputs("READ ", fp);
				writeOp(ir->u.unaryOP.op, fp);
				break;
			case WRITE_IR:
				fputs("WRITE ", fp);
				writeOp(ir->u.unaryOP.op, fp);
				break;
			default:
				break;
		}
		fputs("\n", fp);
		ir = ir->next;
	}
	fclose(fp);
}

void writeOp(Operand op, FILE *fp) {
	if (!op) {
		fputs("t0 ", fp);
		return;
	}
	char str[64];
	memset(str, 0, sizeof(str));
	switch (op->kind)
	{
		case TEMP_VAR_OP:
			sprintf(str, "t%d", op->u.var_no);
			fputs(str, fp);
			break;
		case VARIABLE_OP:
			sprintf(str, "%s", op->u.value);
			fputs(str, fp);
			break;
		case CONSTANT_OP:
			sprintf(str, "#%s", op->u.value);
			fputs(str, fp);
			break;
		case ADDRESS_OP:
			sprintf(str, "*%s", op->u.name);
			fputs(str, fp);
			break;
		case TEMP_ADDRESS_OP:
			sprintf(str, "*t%d", op->u.name);
			fputs(str, fp);
			break;
		case LABEL_OP:
			sprintf(str, "label%d", op->u.label_no);
			fputs(str, fp);
			break;
		case FUNCTION_OP:
			sprintf(str, "%s", op->u.value);
			fputs(str, fp);
			break;
	}
}

