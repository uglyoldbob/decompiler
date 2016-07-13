#ifndef __EXE_LOADER_H__
#define __EXE_LOADER_H__

#include <cstdint>
#include <iostream>

#include "config.h"

class disassembler;

class exe_loader
{
	public:
		exe_loader();
		virtual ~exe_loader();
		virtual int process(std::istream *me) = 0;	//do basic processing
		virtual const char *entry_name() = 0;
		virtual address entry_addr() = 0;
		virtual int goto_address(address addr) = 0;
		virtual void read_memory(void *dest, int len) = 0;
		disassembler *get_disasm();
	protected:
		std::istream *exe;	//allocation not handled in this class
		disassembler *disasm;
		int rbo;
};

#endif