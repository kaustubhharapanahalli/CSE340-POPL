#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include "execute.h"

// addrType specifies how the value of the argument is obtained.
// IMMEDIATE address type is for constants. The value is stored in the data 
//           structure itself and not in memory.
// DIRECT    address type is for scalar variables. The value is stored in 
//           memory and the value in the data structure is the memory address 
//           where the value can be found
// INDIRECT  address type is for array variables. The value is stored in memory
//           but the the address is calculated at runtime. The value in the 
//           data structure is the address of the location that contains the 
//           address of the location where the value is stored. For example, 
//           if we need to access a[i], then the address of a[i] is equal to 
//           the address of a + i which is a runtime quantity.
//           the value of addr_a + i is stored in some memory location whose 
//           address is addr_m. The value of a[i] is at mem[mem[addrm_m]]
vector<string> addrType {"IMMEDIATE", "DIRECT", "INDIRECT", "NOAT", "NA"};

// opType specifies the type of the operator.
// NOOP  there is no operator
// PLUS  the operator is +
// MINUS the operator is -
// MULT  the operator is *
// DIV   the operator is /
// NOOPT the operator value is not initialized
vector<string> opType   { "NOOP", "PLUS", "MINUS", "MULT", "DIV", "NOOPT"};

// instType specifies the type of the instruction
// OUTPUT for output statement
// ASSIGN for an assign instruction
// CJMP   this is a conditional jump. Depending on the evaluation of a 
//        condition, the next instruction to be executed is either the next 
//        instruction or the target of the jump
vector<string> instType { "OUTPUT", "ASSIGN", "CJMP", "NOIT"};


bool valid_instType(InstType it)
{
	return ((it == OUTPUT_INST) | (it == ASSIGN_INST) | (it == NOIT));
}

bool valid_addrType(AddrType at)
{
	return ((at == IMMEDIATE) | (at == DIRECT) | 
		(at == INDIRECT) | (at == NOAT) | (at == NA));
}

void print_instNode(struct instNode *il)
{
	if (valid_instType(il->iType))
		cout << "InstType : " << instType[il->iType] << endl;
	else
		cout << "InstType : " << " not valid " << endl;

	if (valid_addrType(il->lhsat))
		cout << "LHS AddrType : " << addrType[il->lhsat] << endl;
	else
		cout << "LHS AddrType : " << " not valid " << endl;

	cout << "LHS : " << il->lhs << endl;

	if (valid_addrType(il->op1at))
		cout << "OP1 AddrType : " << addrType[il->op1at] << endl;
	else
		cout << "OP1 AddrType : " << " not valid" << endl;

	cout << "OP1 : " << il->op1 << endl;

	if (valid_addrType(il->op1at))
		cout << "OP2 AddrType : " << addrType[il->op2at] << endl;
	else
		cout << "OP2 AddrType : " << " not valid " << endl;

	cout << "OP2 : " << il->op2 << endl;

	cout << "Operator : " << opType[il->oper] << endl;
	cout << "Next : " << il->next << endl;
}

long mem[MEMSIZE];


void execute_inst_list(struct instNode * il)
{
	struct instNode * pc = il;

	while (pc != nullptr) {
		long lhsadr, op1, op2;
		lhsadr = op1 = op2 = 0;
		lhsadr = pc->lhs;
		if ((outofbound(pc->lhs))&(pc->iType == ASSIGN_INST)) {
			cout << "out of bound memory access" << 
				" pc->lhs " << pc->lhs << endl;
			exit(-1);
		}
		if (pc->iType == ASSIGN_INST) {
			switch (pc->lhsat) {
				case DIRECT: lhsadr = pc->lhs;
					break;
				case INDIRECT: lhsadr = mem[pc->lhs];
				  	break;
				case NOAT: cout << "lhs address type not initialized" 
				   		<< endl;
				  	break;
				default: cout << "invalid addressing mode lhs" << endl;
			}
			if (outofbound(lhsadr) ) {
				cout << "out of bound memory access" << 
					" lhsadr " << lhsadr << endl;
				exit(-1);
			}
		}
		if (outofbound(pc->op1)&(pc->op1at != IMMEDIATE)) {
			cout << "out of bound memory access" << 
				" pc->op1 " << pc->op1 << endl;
			exit(-1);
		}
		switch (pc->op1at) {
			case IMMEDIATE: op1 = pc->op1;
				break;
			case DIRECT: op1 = mem[pc->op1];
				break;
			case INDIRECT: if (outofbound(mem[pc->op1])) {
						cout << "out of bound memory access" << 
							" mem[pc->op1] " << mem[pc->op1] << endl;
						exit(-1);
					}
				        op1 = mem[mem[pc->op1]];
				  break;
			case NOAT: cout << "op1 address type not initialized" 
				   	<< endl;
			default: cout << "invalid addressing mode lhs" << endl;
		}

		if ((pc->oper == OP_PLUS)|(pc->oper == OP_MINUS)|
		    (pc->oper == OP_MULT)|(pc->oper == OP_DIV)) {
			if (outofbound(pc->op2)&(pc->op2at != IMMEDIATE)) {
				cout << "out of bound memory access" << 
					" pc->op2 " << pc->op2 << endl;
				exit(-1);
			}
			switch (pc->op2at) {
				case IMMEDIATE: op2 = pc->op2;
					break;
				case DIRECT: op2 = mem[pc->op2];
					break;
				case INDIRECT: if (outofbound(mem[pc->op2])) {
							cout << "out of bound memory access" << 
								" mem[pc->op2] " << mem[pc->op2] << endl;
							exit(-1);
						}
					        op2 = mem[mem[pc->op2]];
				  	break;
				case NA: break;
				case NOAT: cout << "op2 address type not initialized" 
				   		<< endl;
				default: cout << "invalid addressing mode lhs" << endl;
			}
		}

		if ((pc->iType == CJMP_INST)& outofbound(pc->cjmpVar))  {
			cout << "out of bound memory access" << "cjmpVar" << pc->cjmpVar << endl;
			exit(-1);
		}

		switch (pc->iType) {
			case OUTPUT_INST: cout << op1 << endl;
					  pc = pc->next;
					  break;
			case ASSIGN_INST: switch (pc->oper) {
						case OP_NOOP: mem[lhsadr] = op1;
							  break;
						case OP_PLUS: mem[lhsadr] = op1+op2;
							  break;
						case OP_MINUS:mem[lhsadr] = op1-op2;
						   	break;
						case OP_MULT: mem[lhsadr] = op1*op2;
						   	break;
						case OP_DIV:  mem[lhsadr] = op1/op2;
						   	break;
						case OP_NOOPT: cout << "Operator Not Initialized" 
						    		<< endl;
						   	break;
						}
					pc = pc->next;
					break;
			case CJMP_INST:	if (mem[pc->cjmpVar] == 0)
						pc = pc->cjmpTarget;
					else
						pc = pc->next;
					break;
			case NOIT:	cout << "Instruction Type Note Initialized" << endl;;
					pc = nullptr;
					break;
			default:	cout << "Invalid Instruction Type" << endl;;
					pc = nullptr;
		}
	}
}


