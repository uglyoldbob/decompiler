#include "exe_loader.h"

exe_loader::exe_loader()
{
	exe = 0;
	rbo = 0;
}

exe_loader::~exe_loader()
{
	//exe is closed by an upper level class
}

disassembler *exe_loader::get_disasm()
{
	return disasm;
}