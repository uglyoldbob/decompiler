#ifndef __DISASS_PPC_H__
#define __DISASS_PPC_H__

#include "disassembler.h"

class disass_ppc : public disassembler
{
	public:
		disass_ppc(exe_loader *own);
		~disass_ppc();
		instr *get_instruction(address addr);
	private:
};

#endif