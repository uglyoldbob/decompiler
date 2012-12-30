#include "exe_loader.h"

#include "disassembler.h"

exe_loader::exe_loader()
{
	exe = 0;
	rbo = 0;
	disasm = 0;
}

exe_loader::~exe_loader()
{
	//exe is closed by an upper level class
	delete disasm;
}

disassembler *exe_loader::get_disasm()
{
	return disasm;
}
