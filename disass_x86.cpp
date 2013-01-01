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

int disass_x86::get_instruction(instr* &get, address addr)
{
	std::cout << "STUB Get instruction at " << std::hex << addr << std::dec << "\n";
	throw invalid_instruction(addr);

	return 0;
}

std::string disass_x86::trace_value(std::string &val)
{
	return "";
}
