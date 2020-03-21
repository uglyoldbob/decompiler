#include "exe_loader.h"

#include "disassembly/disassembler.h"

std::vector<exe_checker> *exe_loader::checker = 0;

void exe_loader::register_checker(exe_checker a)
{
	if (exe_loader::checker == 0)
	{
		exe_loader::checker = new std::vector<exe_checker>();
	}
	exe_loader::checker->push_back(a);
}

exe_loader * exe_loader::check(std::shared_ptr<std::ifstream> me)
{
	exe_loader *ret = 0;
	if (exe_loader::checker != 0)
	{
		for (unsigned int i = 0; i < exe_loader::checker->size(); i++)
		{
			ret = ((*exe_loader::checker)[i])(me);
			if (ret != 0)
			{
				break;
			}
		}
	}
	return ret;
}

exe_loader::exe_loader(int reverse)
{
	exe = 0;
	rbo = reverse;
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
