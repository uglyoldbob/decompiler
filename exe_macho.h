#ifndef __EXE_MACHO_H__
#define __EXE_MACHO_H__

#include <stdint.h>
#include <stdio.h>

#include "exe_loader.h"

const uint32_t EXE_MACHO_CPU_X86		0x00000007
const uint32_t EXE_MACHO_CPU_X64		0x01000007
const uint32_t EXE_MACHO_CPU_PPC32		0x00000012
const uint32_t EXE_MACHO_CPU_PPC64		0x01000012

const uint32_t EXE_MACHO_FILETYPE_EXE 2

struct exe_macho_header
{
	uint32_t magic;
	uint32_t cputype;
	uint32_t cpusubtype;
	uint32_t filetype;
	uint32_t ncmds;
	uint32_t sizeofcmds;
	uint32_t flags;
};

const uint32_t EXE_MACHO_CMD_SEGMENT		1
const uint32_t EXE_MACHO_CMD_SYMSTAB		2	//stab symbol table
const uint32_t EXE_MACHO_CMD_SYMGDB			3	//gdb symbol table
const uint32_t EXE_MACHO_CMD_THREAD			4
const uint32_t EXE_MACHO_CMD_UNIXTHREAD	5	//with stack
const uint32_t EXE_MACHO_CMD_ROUTINES		17	//image routines?
const uint32_t EXE_MACHO_CMD_SUB_FRAME		18
const uint32_t EXE_MACHO_CMD_SUB_UMBRL		19
const uint32_t EXE_MACHO_CMD_SUB_CLIENT	20
const uint32_t EXE_MACHO_CMD_SUB_LIBR		21
const uint32_t EXE_MACHO_CMD_REQ_DYL		0x80000000	//required to be understood by dynamic linker (flag)

struct exe_macho_load_command
{
	uint32_t cmd;
	uint32_t cmdsize;
	uint8_t *data;
};

class exe_macho : public exe_loader
{
	public:
		exe_macho();
		~exe_macho();
		static int check(FILE *me);
		int process(FILE *me);	//do basic processing
		const char *entry_name();
		uint32_t entry_addr();
		int goto_address(uint32_t addr);
		void read_memory(void *dest, int len);
	private:
		exe_macho_header header;
		exe_macho_load_command *lcmds;
};

#endif