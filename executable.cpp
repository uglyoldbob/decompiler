#include "disassembler.h"
#include "executable.h"
#include "exe_elf.h"
#include "exe_macho.h"

executable::executable()
{
	rbo = 0;
	exe_file = new std::ifstream;
	exe_type = EXEC_TYPE_UNKNOWN;
	exe_object = 0;
}

executable::~executable()
{
	delete exe_object;
	delete exe_file;
}

int executable::check_pe(std::istream *me)
{
	unsigned int signature;
	signature = 0;
	me->seekg(0x3C, std::ios::beg);
	me->read((char*)&signature, 4);
	if (me->fail())
	{
		std::cout << "error reading offset location\n";
		return -1;
	}
	me->seekg(signature, std::ios::beg);
	if (me->good())
	{
		me->read((char*)&signature, 4);
		if (signature == 0x4550)
		{
			std::cout << "PE executable detected\n";
			exe_type = EXEC_TYPE_PE;
			return 1;
		}
		else if (signature == 0x00004550)
		{
			std::cout << "BACKWARDS? PE executable detected\n";
			exe_type = EXEC_TYPE_PE;
			rbo = 1;
			return 1;
		}
	}

	return 0;
}

int executable::load(char *bin_name)
{
	int processed = 0;
	exe_file->open(bin_name, std::ios::binary);
	if (!exe_file->is_open())
	{
		std::cout << "Failed to open executable " << bin_name << "\n";
		return -1;
	}

	int reverse;
	if ((reverse = exe_elf::check(exe_file)) != 0)
	{
		exe_type = EXEC_TYPE_ELF;
		exe_object = new exe_elf();
		if (reverse < 0)
			rbo = 1;
	}
	if ((reverse = exe_macho::check(exe_file)) != 0)
	{
		exe_type = EXEC_TYPE_MACHO32;
		exe_object = new exe_macho();
		if (reverse < 0)
			rbo = 1;
	}

	if (exe_type == EXEC_TYPE_UNKNOWN)
	{
		std::cout << "Unknown executable format\n";
		exe_file->close();
		return -1;
	}

	if (exe_object == 0)
	{
		exe_file->close();
		return -1;
	}
	if (exe_object->process(exe_file) == 0)
	{
		std::cout << "Successfully processed the executable\n";
	}
	else
	{
		exe_file->close();
		return -1;
	}

	function *themain;
	themain = new function(exe_object->entry_addr(), exe_object->entry_name());
	funcs.push_back(themain);

	std::cout << "Created entry point function " << funcs[0]->get_name()
		 << " at address " << std::hex << funcs[0]->gets() << "\n";

	handle_function(0);

	exe_file->close();
	return processed;
}

void executable::handle_function(int i)
{
	std::cout << "STUB Add lines of code to function " << std::hex << funcs[i]->gets() << "\n";
	std::vector<address> blocks;	//indicates the starting point of blocks
	blocks.push_back((uint32_t)funcs[i]->gets());
	while (blocks.size() > 0)
	{
		std::cout << "Working on block starting at " << std::hex << blocks[0] << "\n";
		//does this block already exist?
		exe_object->get_disasm()->get_instruction(blocks[0]);	//gather an instruction
		//add it to the block
		//if the next instruction does not belong to this block, move to another block
		blocks.erase(blocks.begin());
	}
}