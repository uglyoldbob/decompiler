#ifndef __EXE_LOADER_H__
#define __EXE_LOADER_H__

#include <stdint.h>
#include <stdio.h>

class disassembler;

class exe_loader
{
	public:
		exe_loader();
		~exe_loader();
		virtual int process(FILE *me) = 0;	//do basic processing
		virtual const char *entry_name() = 0;
		virtual uint32_t entry_addr() = 0;
		virtual int goto_address(uint32_t addr) = 0;
		virtual void read_memory(void *dest, int len) = 0;
	protected:
		FILE *exe;
		disassembler *disasm;
};

#endif