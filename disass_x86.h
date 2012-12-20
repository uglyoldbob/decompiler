#ifndef __DISASS_X86_H__
#define __DISASS_X86_H__

#include "config.h"
#include "disassembler.h"

class disass_x86 : public disassembler
{
	public:
		disass_x86(exe_loader *own);
		~disass_x86();
		instr *get_instruction(address addr);
	private:
};

#endif