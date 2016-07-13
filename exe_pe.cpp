#include "exe_pe.h"

#include "disassembly/disass_ppc.h"
#include "disassembly/disass_x86.h"
#include "exceptions.h"
#include "executable.h"

exe_pe::exe_pe()
{
	rbo = 0;
}

exe_pe::~exe_pe()
{
}

int exe_pe::check(std::istream *me)
{
	unsigned int signature;
	signature = 0;
	me->seekg(0, std::ios::beg);
	if (me->good())
	{
		me->read((char*)&signature, 2);
		printf("Signature is %x\n", signature);
		if (signature == EXE_PE_ID)
		{ 
			return 1;
		}
	}
	return 0;
}

const char *exe_pe::entry_name()
{
	return "_start";
}

int exe_pe::process(std::istream *me)	//do basic processing
{
	exe = me;
	exe->seekg(0, std::ios::beg);
	if (exe->good())
	{
		exe->read((char*)&header, sizeof(header));
		if (header.signature == EXE_PE_ID)
		{
			size = (header.blocks_in_file-1)*512;
			if (header.bytes_in_last_block == 0)
			{
				size += 512;
			}
			else
			{
				size += header.bytes_in_last_block;
			}
			printf("CS: 0x%x IP: 0x%x\n", header.cs, header.ip);
			printf("SS: 0x%x SP:0x%x\n", header.ss, header.sp);
			//ss should be adjusted by header.header_paragraphs
			//cs should be adjusted by header.header_paragraphs
			
			printf("There are %d paragraphs\n", header.header_paragraphs);
			printf("BSS Size %d to %d paragraphs\n", header.min_extra_paragraphs, header.max_extra_paragraphs);
			printf("Header size is 0x%x\n", header.header_paragraphs * 16);
			printf("Program size: 0x%x [0x%x, 0x%x]\n", size,
				header.blocks_in_file, header.bytes_in_last_block);
			printf("Number of relocations: %d\n", header.num_relocs);
			printf("Relocation table offset: 0x%x\n", header.reloc_table_offset);
			printf("Overlay number: 0x%x\n", header.overlay_number);
			exe->seekg(header.reloc_table_offset, std::ios::beg);
			for (int i = 0; i < header.num_relocs; i++)
			{
				exe_pe_reloc temp;
				exe->read((char*)&temp, sizeof(temp));
				//printf("Relocation %d, offset 0x%x, segment 0x%x\n", i, temp.offset, temp.segment);
				//add image start (0) to variable at temp.segment::temp.offset 
			}
			starting = header.ip + header.cs*0x10;
			disasm = new disass_x86(this);
			return 0;
		}
	}
	return 0;
}

int exe_pe::goto_address(address addr)
{
	int bad = 1;
	if (addr <= size)
	{
		bad = 0;
		exe->seekg(addr+header.header_paragraphs*16, std::ios::beg);
	}
	if (bad)
		throw address_not_present(addr);
	return 0;
}

address exe_pe::entry_addr()
{
	return starting;
}

void exe_pe::read_memory(void *dest, int len)
{
	exe->read((char*)dest, len);
	switch (len)
	{
		case sizeof(uint16_t):
			reverse((uint16_t*)dest, rbo);
			break;
		case sizeof(uint32_t):
			reverse((uint32_t*)dest, rbo);
			break;
		case sizeof(uint64_t):
			reverse((uint64_t*)dest, rbo);
			break;
		default:
			break;
	}
}
