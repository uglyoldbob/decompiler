#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

#include "config.h"
#include "variable.h"
#include <iostream>
#include <string>
#include <vector>

struct instr
{
	address addr;
	std::string preprint;
	std::vector<variable *> statements;
	std::string comment;
	int ins;	//increase when control is branched to here
	int is_cbranch;	//set when it is a conditional branch statement
	char len;	//length of the instruction
	address destaddra;	//stores the dest of the conditional branch statement
	address destaddrb;	//stores the other dest
	address call;		//a literal value for a function call
	std::string trace_call;		//a function call whose address must be traced
	std::string trace_jump;		//a jump address that must be traced
};
//only two possibilities for branching
	//non-literal function calls are easier to trace values for becuase all the structure is present
	//non-literal branches could refer to many things
		//could branch to the calling function (return bla;)
		//could branch to

std::ostream& operator<< (std::ostream& out, instr &a);

class exe_loader;

class disassembler
{
	public:
		disassembler(exe_loader *own);
		virtual ~disassembler();
		
		virtual int get_instruction(instr* &get, address addr) = 0;
		virtual std::string trace_value(std::string &val) = 0;
	protected:
		exe_loader *owner;
};

#endif
