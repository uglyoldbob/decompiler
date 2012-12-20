#include "disass_x86_32.h"

#include <stdio.h>

disass_x86_32::disass_x86_32(exe_loader *own) :
	disassembler(own)
{
}

disass_x86_32::~disass_x86_32()
{
}

instr* disass_x86_32::get_instruction(void *addr)
{
	instr *ret = 0;
	printf("STUB Get instruction at %x\n", addr);


	return ret;
}
