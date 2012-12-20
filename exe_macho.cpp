#include "executable.h"
#include "exe_macho.h"

#include "disass_x86.h"

exe_macho::exe_macho()
{
	rbo = 0;
	lcmds = 0;
}

exe_macho::~exe_macho()
{
	if (lcmds != 0)
	{
		for (int i = 0; i < header.ncmds; i++)
		{
			if (lcmds[i].data != 0)
				delete [] lcmds[i].data;
		}
		delete [] lcmds;
	}
}

int exe_macho::check(FILE *me)
{
	unsigned int signature;
	signature = 0;
	fseek(me, 0, SEEK_SET);
	if (ferror(me) == 0)
	{
		fread(&signature, 4, 1, me);
		if (signature == 0xFEEDFACE)
		{
			printf("32-bit MACH-O executable detected\n");
			return 1;
		}
		else if (signature == 0xCEFAEDFE)
		{
			printf("BACKWARDS 32-bit MACH-O executable detected\n");
			return -1;
		}
	}
	return 0;
}

const char *exe_macho::entry_name()
{
	return "main";
}

int exe_macho::process(FILE *me)	//do basic processing
{
	exe = me;
	fseek(exe, 0, SEEK_SET);
	if (ferror(exe) == 0)
	{
		fread(&header, sizeof(header), 1, exe);
		if (header.magic == 0xCEFAEDFE)
		{
			rbo = 1;
			reverse(&header.cputype, rbo);
			reverse(&header.cpusubtype, rbo);
			reverse(&header.filetype, rbo);
			reverse(&header.ncmds, rbo);
			reverse(&header.sizeofcmds, rbo);
			reverse(&header.flags, rbo);
		}
	}
	printf("STUB Doing processing for a ");
	if (rbo == 1)
		printf("(reverse) ");
	printf("MACHO executable\n");
	switch (header.cputype)
	{
		case EXE_MACHO_CPU_PPC32:
			break;
		default:
			printf("Unsupported cpu type 0x%x\n");
			return -1;
	}

	//I don't think the subtype of the CPU matters
		//386, 486, etc
		//powerpc 601, 602, etc

	if (header.filetype != EXE_MACHO_FILETYPE_EXE)
	{
		printf("Unsupported filetype 0x%x\n", header.filetype);
	}

	printf("Flags is %08x\n", header.flags);

	printf("There are %d commands of size 0x%x\n", header.ncmds, header.sizeofcmds);
	lcmds = new exe_macho_load_command[header.ncmds];
	for (int i = 0; i < header.ncmds; i++)
	{
		fread(&lcmds[i].cmd, sizeof(uint32_t), 1, exe);
		fread(&lcmds[i].cmdsize, sizeof(uint32_t), 1, exe);
		reverse(&lcmds[i].cmd, rbo);
		reverse(&lcmds[i].cmdsize, rbo);
		if (lcmds[i].cmdsize > 8)
		{
			lcmds[i].data = new uint8_t[lcmds[i].cmdsize-8];
			fread(lcmds[i].data, lcmds[i].cmdsize-8, 1, exe);
		}
		else
		{
			lcmds[i].data = 0;
		}
	}
	printf("Load commands are loaded\n");

	

	printf("Not quite done with the MACHO processing\n");
	return -1;
}

int exe_macho::goto_address(address addr)
{
	return 0;
}

address exe_macho::entry_addr()
{
	return 0;
}

void exe_macho::read_memory(void *dest, int len)
{
	fread(dest, len, 1, exe);
}