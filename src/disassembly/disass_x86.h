#ifndef __DISASS_X86_H__
#define __DISASS_X86_H__

#include "config.h"
#include "disassembler.h"

#include "udis86.h"

/*! \brief The disassembler for x86 (16 and 32 bit) */
class disass_x86 : public disassembler
{
	public:
		disass_x86(exe_loader *own);
		~disass_x86();
		int get_instruction(instr* &get, address addr); 
	private:
		ud_t u;
		address shift_cs;
		address shift_ss;

		statement *interpret_operand(const ud_operand_t *m);
};

#endif
