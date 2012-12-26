#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

#include "config.h"
#include <string>

struct instr
{
	address addr;
	std::string opcode;
	std::string options;
	std::string comment;
	unsigned char length;
	int ins;	//increase when control is branched to here
	int is_cbranch;	//set when it is a conditional branch statement
	address destaddra;	//stores the dest of the conditional branch statement
	address destaddrb;	//stores the other dest
	int line_num;	//makes it easier for a code block to find the first line of the next block
};

class exe_loader;

class disassembler
{
	public:
		disassembler(exe_loader *own);
		virtual ~disassembler();
		
		virtual instr *get_instruction(address addr) = 0;
	protected:
		exe_loader *owner;
};

#endif