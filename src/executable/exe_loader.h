#ifndef __EXE_LOADER_H__
#define __EXE_LOADER_H__

#include <cstdint>
#include <iostream>
#include <vector>

#include "config.h"

#include "code_elements/code_element.h"

class disassembler;
class exe_loader;
typedef exe_loader* (*exe_checker)(std::istream *me);

/*
The exe_loader class is responsible for loading things from the target
executable. Each specific type of object has its own class that inherits
from this base class
*/
class exe_loader
{
	public:
		exe_loader(int reverse);
		virtual ~exe_loader();
		virtual int process(std::istream *me) = 0;	//do basic processing
		virtual const char *entry_name() = 0;
		virtual address entry_addr() = 0;
		virtual int goto_address(address addr) = 0;
		virtual void read_memory(void *dest, int len) = 0;
		disassembler *get_disasm();
		virtual std::vector<code_element *> gather_instructions(address start_address) = 0;
		static void register_checker(exe_checker a);
		static exe_loader* check(std::istream *me);
	protected:
		std::istream *exe;	//allocation not handled in this class
		disassembler *disasm;
		int rbo;
		static std::vector<exe_checker> *checker;
};

#endif