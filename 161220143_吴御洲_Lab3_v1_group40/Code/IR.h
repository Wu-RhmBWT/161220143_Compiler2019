#ifndef __IR_H__
#define __IR_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct Operand_* Operand;
typedef struct Operand_ {
	enum {
		TEMP_VAR_OP, VARIABLE_OP, CONSTANT_OP, ADDRESS_OP, LABEL_OP, FUNCTION_OP
	} kind;
	union {
		int var_no;		//TEMP_VAR_OP
		int label_no;	//LABEL_OP
		char* value;	//VARIABLE_OP CONSTANT_OP FUNCTION_OP
		Operand name;	//ADDRESS_OP
	} u;
	Operand next;
}a;

typedef struct InterCode_* InterCode;
typedef struct InterCode_ {
	enum {
			LABEL_IR, FUNCTION_IR, ASSIGN_IR, PLUS_IR, SUB_IR, MUL_IR, DIV_IR, GET_ADDR_IR, GET_VALUE_IR, 
			TO_MEMORY_IR, GOTO_IR, IF_GOTO_IR, RETURN_IR, DEC_IR, ARG_IR, CALL_IR, PARAM_IR, READ_IR, WRITE_IR
	} kind;
	union{
		//LABEL_IR FUNCTION_IR GOTO_IR RETURN_IR ARG_IR PARAM_IR READ_IR WRITE_IR DEBUG_IR
		struct {
			Operand op;
		} unaryOP;
		
		//ASSIGN_IR GET_VALUE_IR TO_MEMORY_IR CALL_IR
		struct {
			Operand left;
			Operand right;
		} binaryOP;
		
		//PLUS_IR SUB_IR MUL_IR DIV_IR GET_ADDR_IR
		struct {
			Operand result;
			Operand op1;
			Operand op2;
		} ternaryOP;
		
		//IF_GOTO_IR
		struct {
			Operand op1;
			Operand op2;
			Operand label;
			char relop[32];
		} ifgotoOP;
		
		//DEC_IR
		struct {
			Operand op;
			int size;
		} decOP;
	} u;
	InterCode pre, next;
}b;

void initIRList();
void insertCode(InterCode insert);
void deleteCode(InterCode del);
void writeCode(char *filename);
void writeOp(Operand op, FILE *file);
void printCode();
void printOp(Operand op);
#endif
