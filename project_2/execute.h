#ifndef _EXECUTE
#define _EXECUTE
const int MEMSIZE = 10000;

#define outofbound(index)  ((index < 0) | (index >= MEMSIZE))

using namespace std;

enum AddrType {IMMEDIATE=0, DIRECT, INDIRECT, NOAT, NA};
enum OpType {OP_NOOP=0, OP_PLUS, OP_MINUS, OP_MULT, OP_DIV,OP_NOOPT};
enum InstType {OUTPUT_INST=0, ASSIGN_INST, CJMP_INST, NOIT};

struct instNode {
	InstType iType;
	AddrType lhsat;
	long lhs;
	long cjmpVar;   // index of variable for cjmp condition
			// jmp occurs if equal to zero
			// has no addressing type specified 
			// because it is always DIRECT addressed
	AddrType  op1at;
	long op1;
	AddrType  op2at;
	OpType oper;
	long op2;
	struct instNode * cjmpTarget;
	struct instNode * next;

	instNode() {
		iType=NOIT;
		lhsat=NOAT; lhs = -1;
		cjmpVar = -1;
		op1at=NOAT; op1 = -1;
		oper = OP_NOOPT;
		op2at=NOAT; op2= -1;
		cjmpTarget = nullptr;
		next = nullptr;
	}
};

bool valid_instType(InstType);
bool valid_addrType(AddrType);
void print_instNode(struct instNode *);
void execute_inst_list(struct instNode *);

#endif
