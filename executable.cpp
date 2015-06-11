#include "disassembler.h"
#include "exceptions.h"
#include "executable.h"
#include "exe_elf.h"
#include "exe_macho.h"
#include "exe_pe.h"

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
	if ((reverse = exe_pe::check(exe_file)) != 0)
	{
		exe_type = EXEC_TYPE_PE;
		exe_object = new exe_pe();
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

	function *start;
	std::vector<address> function_addresses;	//a list of function start addresses
	start = new function(exe_object->entry_addr(), exe_object->entry_name(), *(exe_object->get_disasm()));
	funcs.push_back(start);
	std::vector<address> temp = funcs.back()->get_calls();
	std::cout << std::hex;
	for (unsigned int i = 0; i < temp.size(); i++)
	{
		if (check_func_list(temp[i]))
		{
			std::cout << "\tNew function at 0x" << temp[i] << std::endl;
			function_addresses.push_back(temp[i]);
		}
	}
	std::cout << std::dec;
	while (function_addresses.size() > 0)
	{
		std::stringstream name;
		name << "func_" << std::hex << function_addresses[0] << std::dec;
		function *tfunc = new function(function_addresses[0], name.str().c_str(), *(exe_object->get_disasm()));
		funcs.push_back(tfunc);
		function_addresses.erase(function_addresses.begin());
		std::vector<address> temp = funcs.back()->get_calls();
		std::cout << std::hex;
		for (unsigned int i = 0; i < temp.size(); i++)
		{
			if (check_func_list(temp[i]))
			{
				std::cout << "\tNew function at 0x" << temp[i] << std::endl;
				function_addresses.push_back(temp[i]);
			}
		}
	}

	exe_file->close();
	for (int i = 0; i < funcs.size(); i++)
	{
		delete (funcs[i]);
	}
	return processed;
}

int executable::check_func_list(address addr)
{
	for (unsigned int i = 0; i < funcs.size(); i++)
	{
		if (funcs[i]->gets() == addr)
			return 0;
	}
	return 1;
}
