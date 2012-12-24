#include "disass_ppc.h"

#include <iostream>

#include "disassembler.h"
#include "exe_loader.h"
#include "exceptions.h"

disass_ppc::disass_ppc(exe_loader *own)
	: disassembler(own)
{
}

disass_ppc::~disass_ppc()
{
}

instr* disass_ppc::get_instruction(address addr)
{
	instr *ret = 0;
	owner->goto_address(addr);
	std::cout << "STUB Get instruction at " << std::hex << addr << "\n";
	throw invalid_instruction(addr);

	return ret;
}
