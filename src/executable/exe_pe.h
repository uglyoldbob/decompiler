#ifndef __EXE_PE_H__
#define __EXE_PE_H__

#include <cstdint>
#include <iostream>

#include "config.h"
#include "exe_loader.h"
#include "exe_real.h"

#define EXE_PE_ID 0x5A4D

struct exe_pe_header
{
	unsigned short signature; /* == 0x5a4D */
	unsigned short bytes_in_last_block;
	unsigned short blocks_in_file;
	unsigned short num_relocs;
	unsigned short header_paragraphs;
	unsigned short min_extra_paragraphs;
	unsigned short max_extra_paragraphs;
	unsigned short ss;
	unsigned short sp;
	unsigned short checksum;
	unsigned short ip;
	unsigned short cs;
	unsigned short reloc_table_offset;
	unsigned short overlay_number;
};

struct exe_pe_reloc
{
	unsigned short offset;
	unsigned short segment;
};

#if TARGET32
#elif TARGET64
#endif

class exe_pe : public exe_real
{
	public:
		exe_pe(int reverse);
		~exe_pe();
		static exe_loader* check(std::istream *me);
		int process(std::istream *me);	//do basic processing
		const char *entry_name();
		address entry_addr();
		int goto_address(address addr);
		void read_memory(void *dest, int len);
	private:
		address starting;
		exe_pe_header header;
		unsigned int size;
};

#endif