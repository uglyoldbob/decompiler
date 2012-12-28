#include "disassembler.h"
#include "exceptions.h"
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
		throw file_open_failed(bin_name);
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
		exe_file->close();
		throw unknown_exe_format(bin_name);
	}

	if (exe_object == 0)
	{
		exe_file->close();
		throw unknown_exe_format(bin_name);
	}
	if (exe_object->process(exe_file) != 0)
	{
		exe_file->close();
		throw unknown_exe_format(bin_name);
	}

	function *themain;
	themain = new function(exe_object->entry_addr(), exe_object->entry_name(), *(exe_object->get_disasm()));
	funcs.push_back(themain);
	std::vector<address> temp = funcs[0]->get_calls();

	std::cout << std::hex;
	for (unsigned int i = 0; i < temp.size(); i++)
	{
		std::cout << "\t0x" << temp[i] << std::endl;
	}
	std::cout << std::dec;

	exe_file->close();
	return processed;
}
