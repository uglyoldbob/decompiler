#include "exe_macho.h"

#include "disassembly/disass_ppc.h"
#include "disassembly/disass_x86.h"
#include "exceptions.h"
#include "executable.h"

/*! \brief Helper class to register executable format classes
 *
 * This class is used to automatically register the executable format parser. */
class register_exe_macho
{
	public:
		register_exe_macho()
		{
			exe_loader::register_checker(exe_macho::check);
		}
};

static register_exe_macho make_it_so;

exe_macho::exe_macho(int reverse) : exe_real(reverse)
{
	lcmds = 0;
}

exe_macho::~exe_macho()
{
	if (lcmds != 0)
	{
		for (uint32_t i = 0; i < header.ncmds; i++)
		{
			switch (lcmds[i].cmd)
			{
				case EXE_MACHO_CMD_SEGMENT:
					delete [] lcmds[i].data.seg.sections;
					break;
				default:
					break;
			}
		}
	}
	delete [] lcmds;
}

exe_loader * exe_macho::check(std::shared_ptr<std::ifstream> me)
{
	exe_loader *ret = 0;
	unsigned int signature;
	signature = 0;
	me->seekg(0, std::ios::beg);
	if (me->good())
	{
		me->read((char*)&signature, 4);
		if (signature == EXE_MACHO_MAGIC_32)
		{
			ret = new exe_macho(0);
		}
		else if (signature == EXE_MACHO_CIGAM_32)
		{
			ret = new exe_macho(1);
		}
		else if (signature == EXE_MACHO_MAGIC_64)
		{
			ret = new exe_macho(0);
		}
		else if (signature == EXE_MACHO_CIGAM_64)
		{
			ret = new exe_macho(1);
		}
	}
	return ret;
}

const char *exe_macho::entry_name()
{
	return "_start";
}

int exe_macho::process(std::shared_ptr<std::ifstream> me)	//do basic processing
{
	exe = me;
	exe->seekg(0, std::ios::beg);
	if (exe->good())
	{
		exe->read((char*)&header, sizeof(header));
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
			std::cout << "Mach-O Not a 32 bit executable\n";
#elif TARGET64
			std::cout << "Mach-O Not a 64 bit executable\n";
#endif
			return -1;
		}
	}
	switch (header.cputype)
	{
		case EXE_MACHO_CPU_PPC:
			break;
		case EXE_MACHO_CPU_X86:
		default:
			std::cout << "Unsupported cpu type 0x" << std::hex << header.cputype << std::dec << "\n";
			return -1;
	}

	//I don't think the subtype of the CPU matters
		//386, 486, etc
		//powerpc 601, 602, etc

	if (header.filetype != EXE_MACHO_FILETYPE_EXE)
	{
		std::cout << "Unsupported filetype 0x" << std::hex << header.filetype << std::dec << "\n";
	}

	lcmds = new exe_macho_lc[header.ncmds];
	for (uint32_t i = 0; i < header.ncmds; i++)
	{
		exe->read((char*)&lcmds[i].cmd, sizeof(uint32_t));
		exe->read((char*)&lcmds[i].cmdsize, sizeof(uint32_t));
		reverse(&lcmds[i].cmd, rbo);
		reverse(&lcmds[i].cmdsize, rbo);
		if (lcmds[i].cmdsize > 8)
		{
			switch (lcmds[i].cmd)
			{
				case EXE_MACHO_CMD_SEGMENT:
					exe->read((char*)&lcmds[i].data.seg, sizeof(exe_macho_lc_segment));
					reverse(&lcmds[i].data.seg.vmaddr, rbo);
					reverse(&lcmds[i].data.seg.vmsize, rbo);
					reverse(&lcmds[i].data.seg.fileoff, rbo);
					reverse(&lcmds[i].data.seg.filesize, rbo);
					reverse(&lcmds[i].data.seg.maxprot, rbo);
					reverse(&lcmds[i].data.seg.initprot, rbo);
					reverse(&lcmds[i].data.seg.nsects, rbo);
					reverse(&lcmds[i].data.seg.flags, rbo);
					exe->seekg((int)(-sizeof(void*)), std::ios::cur);
					if (lcmds[i].data.seg.nsects > 0)
					{
						lcmds[i].data.seg.sections = new exe_macho_lc_section[lcmds[i].data.seg.nsects];
						for (uint32_t j = 0; j < lcmds[i].data.seg.nsects; j++)
						{
							exe->read((char*)&lcmds[i].data.seg.sections[j], sizeof(exe_macho_lc_section));
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
				case EXE_MACHO_CMD_THREAD:
				case EXE_MACHO_CMD_UNIXTHREAD:
					switch (header.cputype)
					{	//thread status depends on architecture
						case EXE_MACHO_CPU_PPC:
							exe->read((char*)&lcmds[i].data.ppc_thread, sizeof(exe_macho_ppc_threadstate));
							reverse(&lcmds[i].data.ppc_thread.flavor, rbo);
							reverse(&lcmds[i].data.ppc_thread.count, rbo);
							reverse(&lcmds[i].data.ppc_thread.cr, rbo);
							reverse(&lcmds[i].data.ppc_thread.xer, rbo);
							reverse(&lcmds[i].data.ppc_thread.lr, rbo);
							reverse(&lcmds[i].data.ppc_thread.ctr, rbo);
							//don't bother with mq register?
							reverse(&lcmds[i].data.ppc_thread.vrsave, rbo);
							for (int k = 0; k < 32; k++)
								reverse(&lcmds[i].data.ppc_thread.r[k], rbo);
							reverse(&lcmds[i].data.ppc_thread.srr[0], rbo);
							reverse(&lcmds[i].data.ppc_thread.srr[1], rbo);
							starting = lcmds[i].data.ppc_thread.srr[0];
							disasm = new disass_ppc(this);
							break;
						case EXE_MACHO_CPU_X86:
						default:
							std::cout << "Architecture " << header.cputype << " not supported\n";
							break;
					}
					exe->seekg(lcmds[i].cmdsize, std::ios::cur);
					break;
				default:
					exe->seekg(-8, std::ios::cur);	//cmd and cmdsize variables
					exe->seekg(lcmds[i].cmdsize, std::ios::cur);
					break;
			}
		}
	}
	for (uint32_t i = 0; i < header.ncmds; i++)
	{
		switch (lcmds[i].cmd)
		{
			case EXE_MACHO_CMD_SEGMENT:
				if (lcmds[i].data.seg.nsects > 0)
				{
					for (uint32_t j = 0; j < lcmds[i].data.seg.nsects; j++)
					{
						exe->seekg(lcmds[i].data.seg.sections[j].offset, std::ios::beg);
					}
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

int exe_macho::goto_address(address addr)
{
	int bad = 1;
	for (uint32_t i = 0; i < header.ncmds; i++)
	{
		if (lcmds[i].cmd == EXE_MACHO_CMD_SEGMENT)
		{
			for (uint32_t j = 0; j < lcmds[i].data.seg.nsects; j++)
			{
				if ( (addr >= lcmds[i].data.seg.sections[j].addr) &&
					 (addr < (lcmds[i].data.seg.sections[j].addr + lcmds[i].data.seg.sections[j].size)) )
				{
					exe->seekg(lcmds[i].data.seg.sections[j].offset + addr - lcmds[i].data.seg.sections[j].addr, std::ios::beg);
					bad = 0;
				}
			}
		}
	}
	if (bad)
		throw address_not_present(addr);
	return 0;
}

address exe_macho::entry_addr()
{
	return starting;
}

void exe_macho::read_memory(void *dest, int len)
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
