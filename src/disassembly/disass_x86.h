#ifndef __DISASS_X86_H__
#define __DISASS_X86_H__

#include "config.h"
#include "disassembler.h"

#include "udis86.h"

class disass_x86 : public disassembler
{
	public:
		disass_x86(exe_loader *own);
		~disass_x86();
		int get_instruction(instr* &get, address addr); 
		std::string trace_value(std::string &val);
	private:
		ud_t u;
		address shift_cs;
		address shift_ss;
};

#endif
