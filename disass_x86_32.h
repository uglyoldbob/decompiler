#ifndef __DISASS_X86_32_H__
#define __DISASS_X86_32_H__

#include "disassembler.h"

class disass_x86_32 : public disassembler
{
	public:
		disass_x86_32(exe_loader *own);
		~disass_x86_32();
	private:
};

#endif