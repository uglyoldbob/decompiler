#include "disass_x86.h"

#include "config.h"
#include <stdio.h>

disass_x86::disass_x86(exe_loader *own) :
	disassembler(own)
{
}

disass_x86::~disass_x86()
{
}

instr* disass_x86::get_instruction(address addr)
{
	instr *ret = 0;
	printf("STUB Get instruction at %x\n", addr);


	return ret;
}
