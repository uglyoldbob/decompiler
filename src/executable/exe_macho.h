#ifndef __EXE_MACHO_H__
#define __EXE_MACHO_H__

#include <cstdint>
#include <iostream>

#include "config.h"
#include "exe_loader.h"

const uint32_t EXE_MACHO_CPU_X86_32		= 0x00000007;
const uint32_t EXE_MACHO_CPU_X86_64		= 0x01000007;
const uint32_t EXE_MACHO_CPU_PPC32		= 0x00000012;
const uint32_t EXE_MACHO_CPU_PPC64		= 0x01000012;

#if TARGET32
	const uint32_t EXE_MACHO_CPU_X86 = EXE_MACHO_CPU_X86_32;
	const uint32_t EXE_MACHO_CPU_PPC = EXE_MACHO_CPU_PPC32;
#elif TARGET64
	const uint32_t EXE_MACHO_CPU_X86 = EXE_MACHO_CPU_X86_64;
	const uint32_t EXE_MACHO_CPU_PPC = EXE_MACHO_CPU_PPC64;
#endif

const uint32_t EXE_MACHO_FILETYPE_EXE	= 2;

const uint32_t EXE_MACHO_MAGIC_32 = 0xFEEDFACE;
const uint32_t EXE_MACHO_CIGAM_32 = 0xCEFAEDFE;
const uint32_t EXE_MACHO_MAGIC_64 = 0xFEEDFACF;
const uint32_t EXE_MACHO_CIGAM_64 = 0xCFFAEDFE;

#if TARGET32
	const uint32_t EXE_MACHO_MAGIC = EXE_MACHO_MAGIC_32;
	const uint32_t EXE_MACHO_CIGAM = EXE_MACHO_CIGAM_32;
#elif TARGET64
	const uint32_t EXE_MACHO_MAGIC = EXE_MACHO_MAGIC_64;
	const uint32_t EXE_MACHO_CIGAM = EXE_MACHO_CIGAM_64;
#endif

#if TARGET32
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
#elif TARGET64
struct exe_macho_header
{
	uint32_t magic;
	uint32_t cputype;
	uint32_t cpusubtype;
	uint32_t filetype;
	uint32_t ncmds;
	uint32_t sizeofcmds;
	uint32_t flags;
	uint32_t reserved;
};
#endif

const uint32_t EXE_MACHO_CMD_SEGMENT		=1;
const uint32_t EXE_MACHO_CMD_SYMSTAB		=2;	//stab symbol table
const uint32_t EXE_MACHO_CMD_SYMGDB			=3;	//gdb symbol table
const uint32_t EXE_MACHO_CMD_THREAD			=4;
const uint32_t EXE_MACHO_CMD_UNIXTHREAD	=5;	//with stack
const uint32_t EXE_MACHO_CMD_ROUTINES		=17;	//image routines?
const uint32_t EXE_MACHO_CMD_SUB_FRAME		=18;
const uint32_t EXE_MACHO_CMD_SUB_UMBRL		=19;
const uint32_t EXE_MACHO_CMD_SUB_CLIENT	=20;
const uint32_t EXE_MACHO_CMD_SUB_LIBR		=21;
const uint32_t EXE_MACHO_CMD_REQ_DYL		=0x80000000;	//required to be understood by dynamic linker (flag)

typedef unsigned int vm_prot_t;

#ifdef TARGET32
struct exe_macho_lc_section
{
	char sectname[16];
	char segname[16];
	uint32_t addr;
	uint32_t size;
	uint32_t offset;
	uint32_t align;
	uint32_t reloff;
	uint32_t nreloc;
	uint32_t flags;
	uint32_t reserved1;
	uint32_t reserved2;
};
#elif TARGET64
struct exe_macho_lc_section
{
	char sectname[16];
	char segname[16];
	uint64_t addr;
	uint64_t size;
	uint32_t offset;
	uint32_t align;
	uint32_t reloff;
	uint32_t nreloc;
	uint32_t flags;
	uint32_t reserved1;
	uint32_t reserved2;
};
#endif

#ifdef TARGET32
struct exe_macho_lc_segment
{
	char segname[16];
	uint32_t vmaddr;
	uint32_t vmsize;
	uint32_t fileoff;
	uint32_t filesize;
	vm_prot_t maxprot;
	vm_prot_t initprot;
	uint32_t nsects;
	uint32_t flags;
	exe_macho_lc_section *sections;
};
#elif TARGET64
struct exe_macho_lc_segment
{
	char segname[16];
	uint64_t vmaddr;
	uint64_t vmsize;
	uint64_t fileoff;
	uint64_t filesize;
	vm_prot_t maxprot;
	vm_prot_t initprot;
	uint32_t nsects;
	uint32_t flags;
	exe_macho_lc_section *sections;
};
#endif

#if TARGET32
struct exe_macho_ppc_threadstate
{
	uint32_t flavor;
	uint32_t count;	//size of coming data, in number of uint32_t's
	uint32_t srr[2];
	uint32_t r[32];
	uint32_t cr;	//condition register
	uint32_t xer;	//user exception register
	uint32_t lr;	//link register
	uint32_t ctr;	//counter register
	uint32_t mq;	//mq register?
	uint32_t vrsave;//vector save register
};
#elif TARGET64
struct exe_macho_ppc_threadstate
{
	uint32_t flavor;
	uint32_t count;	//size of coming data, in number of uint32_t's
	uint64_t srr[2];
	uint64_t r[32];
	uint32_t cr;	//condition register
	uint64_t xer;	//user exception register
	uint64_t lr;	//link register
	uint64_t ctr;	//counter register
	uint32_t vrsave;//vector save register
};
#endif

union exe_macho_lc_data
{
	exe_macho_lc_segment seg;
	exe_macho_ppc_threadstate ppc_thread;
};

struct exe_macho_lc
{
	uint32_t cmd;
	uint32_t cmdsize;
	exe_macho_lc_data data;
};

class exe_macho : public exe_loader
{
	public:
		exe_macho();
		~exe_macho();
		static int check(std::istream *me);
		int process(std::istream *me);	//do basic processing
		const char *entry_name();
		address entry_addr();
		int goto_address(address addr);
		void read_memory(void *dest, int len);
	private:
		exe_macho_header header;
		exe_macho_lc *lcmds;
		address starting;
};

#endif