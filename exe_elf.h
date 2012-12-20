#ifndef __EXE_ELF_H__
#define __EXE_ELF_H__

#include <stdint.h>
#include <stdio.h>

#include "config.h"
#include "exe_loader.h"

#define EI_NIDENT 16
#define EI_CLASS 4
#define EI_DATA 5

#define ELFCLASS32 1
#define ELFCLASS64 2

#define ELFDATA2LSB 1

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
//processor specific
#define ET_LOPROC 0xFF00
#define ET_HIPROC 0xFFFF

#define EM_386 3

struct exe_elf_header
{
	uint8_t e_ident[EI_NIDENT];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

#define SHN_UNDEF 0
#define SHN_LORESERVE 0xFF00
#define SHN_LOPROC 0xFF00
#define SHN_HIPROC 0xFF1F
#define SHN_ABS 0xFFF1
#define SHN_COMMON 0xFFF1
#define SHN_RESERVE 0xFFFF

#if TARGET32
struct exe_elf_section_header
{
	uint32_t sh_name;
	uint32_t sh_type;
	uint32_t sh_flags;
	uint32_t sh_addr;
	uint32_t sh_offset;
	uint32_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint32_t sh_addralign;
	uint32_t sh_entsize;
};
#elif TARGET64
#error "64-bit target not supported"
#endif

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7FFFFFFF


struct exe_elf_program_header
{
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
};

class exe_elf : public exe_loader
{
	public:
		exe_elf();
		~exe_elf();
		static int check(FILE *me);
		int process(FILE *me);	//do basic processing
		const char *entry_name();
		address entry_addr();
		int goto_address(address addr);
		void read_memory(void *dest, int len);
	private:
		exe_elf_header header;
		exe_elf_section_header *sheaders;
		exe_elf_program_header *pheaders;
		char *string_table;

		void print_program_header(int i);
};

#endif