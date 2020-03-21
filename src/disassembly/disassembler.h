#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

#include "config.h"
#include "statement.h"
#include <iostream>
#include <string>
#include <vector>

/// The basic building block for disassembling machine code.
/** Represents an opcode and also contains information about execution flow. */
struct instr
{
	address addr;
	std::string preprint;
	std::vector<statement *> statements;
	std::string comment;
	int is_cbranch;	//set when it is a conditional branch statement
	bool is_branch;	//is this a branch of any kind?
	bool is_ret;	//is this an instruction to return from a function?
	bool valid;
	char len;	//length of the instruction
	address destaddra;	//stores the dest of the conditional branch statement
	address destaddrb;	//stores the other dest
	address call;		//a literal value for a function call
	statement *trace_call;		//a function call whose address must be traced
	statement *trace_jump;		//a jump address that must be traced
};
//only two possibilities for branching
	//non-literal function calls are easier to trace values for becuase all the structure is present
	//non-literal branches could refer to many things
		//could branch to the calling function (return bla;)
		//could branch to

std::ostream& operator<< (std::ostream& out, instr &a);

class exe_loader;

/// The basic disassembler. Specifies how disassembly should happen for a generic cpu type.
/** The basic disassembler. Specifies how disassembly should happen for a generic cpu type. */
class disassembler
{
	public:
		disassembler(exe_loader *own); ///< Create a disassembler and specify the object used to get binary data from the executable.
		virtual ~disassembler();
		
		virtual int get_instruction(instr* &get, address addr) = 0; ///< Disassemble instructions at the specified address
	protected:
		exe_loader *owner; ///< The object used to actually get data from the executable.
};

#endif
