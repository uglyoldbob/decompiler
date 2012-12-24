#include "exe_elf.h"

#include "disass_x86.h"
#include "exceptions.h"
#include <iostream>

exe_elf::exe_elf()
{
	sheaders = 0;
	pheaders = 0;
	string_table = 0;
}

exe_elf::~exe_elf()
{
	delete [] pheaders;
	delete [] sheaders;
	delete [] string_table;
}

int exe_elf::check(std::istream *me)
{
	unsigned int signature;
	signature = 0;
	me->seekg(0, std::ios::beg);
	if (me->good())
	{
		me->read((char*)&signature, 4);//fread(&signature, 4, 1, me);
		if (signature == 0x464C457F)
		{
			return 1;
		}
		else if (signature == 0x7F454C46)
		{
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
			std::cout << "Program Header " << i << ":\n\tUnused entry\n";
			return;
			break;
		case PT_INTERP:
			std::cout << "Program Header " << i << "\n\tUse an interpreter\n";
			break;
		case PT_LOAD:
			std::cout << "Program Header " << i << "\n\tLoadable segment\n"
					  << "\tVirtual Address " << std::hex <<pheaders[i].p_vaddr << "\n"
					  << "\tSize " << std::hex << pheaders[i].p_memsz << "\n"
					  << "\tSize on disk " << std::hex << pheaders[i].p_filesz << "\n";
			break;
		default:
			break;
	}
}

int exe_elf::process(std::istream *me)	//do basic processing
{
	exe = me;
	exe->seekg(0, std::ios::beg);
	exe->read((char*)&header, sizeof(header));
	if (header.e_version != 1)
		return -1;
	if (header.e_type != ET_EXEC)
		return -1;
	if (header.e_ident[EI_CLASS] != ELFCLASS)
	{
#if TARGET32
		std::cout << "ELF Not a 32-bit exeutable\n";
#elif TARGET64
		std::cout << "ELF Not a 64-bit executable\n";
#endif
		return -1;
	}
	if (header.e_ident[EI_DATA] != ELFDATA2LSB)
		return -1;
	switch (header.e_machine)
	{
		case EM_X86:
			disasm = new disass_x86(this);
			break;
		default:
			std::cout << "Unsupported architecture " << header.e_machine << "\n";
			return -1;
			break;
	}
	if (sizeof(exe_elf_program_header) > header.e_phentsize)
	{
		std::cout << "Error: structure is larger than the ELF executable calls for\n";
		return -1;
	}
	if (header.e_phoff != 0)
	{	//load program header table
		std::cout << "Allocating for " << header.e_phnum << " program header entries\n";
		pheaders = new exe_elf_program_header[header.e_phnum];
		for (int i = 0; i < header.e_phnum; i++)
		{	//seek to the correct location
			exe->seekg(header.e_phoff + i * header.e_phentsize, std::ios::beg);
			//now read the data
			exe->read((char*)&pheaders[i], sizeof(exe_elf_program_header));
//			print_program_header(i);
		}
	}
	if (header.e_shoff != 0)
	{	//load section header table
		std::cout << "Allocating for " << header.e_shnum << " sections\n";
		sheaders = new exe_elf_section_header[header.e_shnum];
		for (int i = 0; i < header.e_shnum; i++)
		{
			exe->seekg(header.e_phoff + i * header.e_shentsize, std::ios::beg);
			exe->read((char*)&sheaders[i], sizeof(exe_elf_section_header));
		}
	}
	return 0;
}

int exe_elf::goto_address(address addr)
{
	int bad = 1;
	for (int i = 0; i < header.e_phnum; i++)
	{
		if ( (addr >= pheaders[i].p_vaddr) &&
			 (addr < (pheaders[i].p_vaddr + pheaders[i].p_memsz)) )
		{
			exe->seekg(pheaders[i].p_offset + addr - pheaders[i].p_vaddr, std::ios::beg);
			bad = 0;
		}
	}
	if (bad)
		throw address_not_present(addr);
	return 0;
}

address exe_elf::entry_addr()
{
	return header.e_entry;
}

void exe_elf::read_memory(void *dest, int len)
{
	exe->read((char*)dest, len);
}