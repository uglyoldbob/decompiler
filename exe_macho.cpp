#include "exe_macho.h"

#include "disass_x86.h"
#include "executable.h"

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
			switch (lcmds[i].cmd)
			{
				case EXE_MACHO_CMD_SEGMENT:
					if (lcmds[i].data.seg.sections != 0)
						delete [] lcmds[i].data.seg.sections;
					break;
				default:
					break;
			}
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
		if (signature == EXE_MACHO_MAGIC_32)
		{
			return 1;
		}
		else if (signature == EXE_MACHO_CIGAM_32)
		{
			return -1;
		}
		else if (signature == EXE_MACHO_MAGIC_64)
		{
			return 1;
		}
		else if (signature == EXE_MACHO_CIGAM_64)
		{
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
		if (header.magic == EXE_MACHO_CIGAM)
		{
			rbo = 1;
			reverse(&header.cputype, rbo);
			reverse(&header.cpusubtype, rbo);
			reverse(&header.filetype, rbo);
			reverse(&header.ncmds, rbo);
			reverse(&header.sizeofcmds, rbo);
			reverse(&header.flags, rbo);
		}
		else if (header.magic != EXE_MACHO_MAGIC)
		{
#if TARGET32
			printf("Mach-O Not a 32 bit executable\n");
#elif TARGET64
			printf("Mach-O Not a 64 bit executable\n");
#endif
			return -1;
		}
	}
	printf("STUB Doing processing for a ");
	if (rbo == 1)
		printf("(reverse) ");
	printf("MACHO executable\n");
	switch (header.cputype)
	{
		case EXE_MACHO_CPU_PPC:
			break;
		case EXE_MACHO_CPU_X86:
		default:
			printf("Unsupported cpu type 0x%x\n", header.cputype);
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
	lcmds = new exe_macho_lc[header.ncmds];
	for (int i = 0; i < header.ncmds; i++)
	{
		fread(&lcmds[i].cmd, sizeof(uint32_t), 1, exe);
		fread(&lcmds[i].cmdsize, sizeof(uint32_t), 1, exe);
		reverse(&lcmds[i].cmd, rbo);
		reverse(&lcmds[i].cmdsize, rbo);
		if (lcmds[i].cmdsize > 8)
		{
			printf("LC %d @ %d 0x%08x, len %d, 0x%08x 0x%08x\n", rbo, lcmds[i].cmd, lcmds[i].cmd,
				   lcmds[i].cmdsize, lcmds[i].cmdsize, sizeof(exe_macho_lc_segment));
			switch (lcmds[i].cmd)
			{
				case EXE_MACHO_CMD_SEGMENT:
					fseek(exe, -8, SEEK_CUR);
					fread(&lcmds[i].data.seg, sizeof(exe_macho_lc_segment), 1, exe);
					reverse(&lcmds[i].data.seg.vmaddr, rbo);
					reverse(&lcmds[i].data.seg.vmsize, rbo);
					reverse(&lcmds[i].data.seg.fileoff, rbo);
					reverse(&lcmds[i].data.seg.filesize, rbo);
					reverse(&lcmds[i].data.seg.maxprot, rbo);
					reverse(&lcmds[i].data.seg.initprot, rbo);
					reverse(&lcmds[i].data.seg.nsects, rbo);
					reverse(&lcmds[i].data.seg.flags, rbo);
					fseek(exe, -sizeof(exe_macho_lc_section *), SEEK_CUR);
					if ((lcmds[i].cmdsize - sizeof(exe_macho_lc_segment) + sizeof(exe_macho_lc_section *)) > 0)
					{
						printf("\t%d sections of size %d (%d)\n", lcmds[i].data.seg.nsects,
							4 + lcmds[i].cmdsize - sizeof(exe_macho_lc_segment),
							(4 + lcmds[i].cmdsize - sizeof(exe_macho_lc_segment)) / lcmds[i].data.seg.nsects
							);
						lcmds[i].data.seg.sections = new exe_macho_lc_section[lcmds[i].data.seg.nsects];
						for (int j = 0; j < lcmds[i].data.seg.nsects; j++)
						{
							fread(&lcmds[i].data.seg.sections[j], sizeof(exe_macho_lc_section), 1, exe);
							reverse(&lcmds[i].data.seg.sections[j].addr, rbo);
							reverse(&lcmds[i].data.seg.sections[j].size, rbo);
							reverse(&lcmds[i].data.seg.sections[j].offset, rbo);
							reverse(&lcmds[i].data.seg.sections[j].align, rbo);
							reverse(&lcmds[i].data.seg.sections[j].reloff, rbo);
							reverse(&lcmds[i].data.seg.sections[j].nreloc, rbo);
							reverse(&lcmds[i].data.seg.sections[j].flags, rbo);
							reverse(&lcmds[i].data.seg.sections[j].reserved1, rbo);
							reverse(&lcmds[i].data.seg.sections[j].reserved2, rbo);
						}
					}
					else
					{
						lcmds[i].data.seg.sections = 0;
					}
					break;
				default:
					fseek(exe, -8, SEEK_CUR);
					fseek(exe, lcmds[i].cmdsize, SEEK_CUR);
					break;
			}
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