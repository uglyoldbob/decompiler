#include "disass_x86.h"

#include "config.h"
#include "exceptions.h"
#include <iostream>

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
	std::cout << "STUB Get instruction at " << std::hex << addr << "\n";
	throw invalid_instruction(addr);

	return ret;
}
