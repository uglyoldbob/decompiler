#include "exe_elf.h"

#include "disass_x86_32.h"

exe_elf::exe_elf()
{
	sheaders = 0;
	pheaders = 0;
	string_table = 0;
}

exe_elf::~exe_elf()
{
	if (pheaders != 0)
		delete [] pheaders;
	if (sheaders != 0)
		delete [] sheaders;
	if (string_table != 0)
		delete [] string_table;
}

int exe_elf::check(FILE *me)
{
	unsigned int signature;
	signature = 0;
	fseek(me, 0, SEEK_SET);
	if (ferror(me) == 0)
	{
		fread(&signature, 4, 1, me);
		if (signature == 0x464C457F)
		{
			printf("ELF executable detected\n");
			return 1;
		}
		else if (signature == 0x7F454C46)
		{
			printf("BACKWARDS? ELF executable detected\n");
			return -1;
		}
	}

	return 0;
}

const char *exe_elf::entry_name()
{
	return "main";
}

void exe_elf::print_program_header(int i)
{
	switch(pheaders[i].p_type)
	{
		case PT_NULL:
			printf("Program Header %d:\n\tUnused entry\n", i);
			return;
			break;
		case PT_INTERP:
			printf("Program Header %d:\n\tUse an interpreter\n", i);
			break;
		case PT_LOAD:
			printf("Program Header %d:\n\tLoadable segment\n", i);
			printf("\tVirtual Address %x\n", pheaders[i].p_vaddr);
			printf("\tSize %x\n", pheaders[i].p_memsz);
			printf("\tSize on disk %x\n", pheaders[i].p_filesz);
			break;
		default:
			break;
	}
}

int exe_elf::process(FILE *me)	//do basic processing
{
	exe = me;
	printf("Doing processing for an ELF executable\n");
	fseek(exe, 0, SEEK_SET);
	fread(&header, sizeof(header), 1, exe);
	if (header.e_version != 1)
		return -1;
	if (header.e_type != ET_EXEC)
		return -1;
	if (header.e_ident[EI_CLASS] != ELFCLASS32)
		return -1;
	if (header.e_ident[EI_DATA] != ELFDATA2LSB)
		return -1;
	switch (header.e_machine)
	{
		case EM_386:
			disasm = new disass_x86_32(this);
			break;
		default:
			printf("Unsupported architecture\n");
			return -1;
			break;
	}
	if (sizeof(exe_elf_program_header32) > header.e_phentsize)
	{
		printf("Error: structure is larger than the ELF executable calls for\n");
		return -1;
	}
	printf("Executable type is supported\n");
	if (header.e_phoff != 0)
	{	//load program header table
		printf("Allocating for %d program header entries\n", header.e_phnum);
		pheaders = new exe_elf_program_header32[header.e_phnum];
		for (int i = 0; i < header.e_phnum; i++)
		{	//seek to the correct location
			fseek(exe, header.e_phoff + i * header.e_phentsize, SEEK_SET);
			//now read the data
			fread(&pheaders[i], sizeof(exe_elf_program_header32), 1, exe);
 			print_program_header(i);
		}
	}
	if (header.e_shoff != 0)
	{	//load section header table
		printf("Allocating for %d sections\n", header.e_shnum);
		sheaders = new exe_elf_section_header32[header.e_shnum];
		for (int i = 0; i < header.e_shnum; i++)
		{
			fseek(exe, header.e_shoff + i * header.e_shentsize, SEEK_SET);
			fread(&sheaders[i], sizeof(exe_elf_section_header32), 1, exe);
		}
	}
	return 0;
}

int exe_elf::goto_address(uint32_t addr)
{
	for (int i = 0; i < header.e_phnum; i++)
	{
		if ( (addr >= pheaders[i].p_vaddr) &&
			 (addr < (pheaders[i].p_vaddr + pheaders[i].p_memsz)) )
		{
		//	printf("Located address %x in program header entry %d file offset %d\n", 
		//		   addr, i, pheaders[i].p_offset + addr - pheaders[i].p_vaddr);
			fseek(exe, pheaders[i].p_offset + addr - pheaders[i].p_vaddr, SEEK_SET);
		}
	}
}

uint32_t exe_elf::entry_addr()
{
	return header.e_entry;
}

void exe_elf::read_memory(void *dest, int len)
{
	fread(dest, len, 1, exe);
}