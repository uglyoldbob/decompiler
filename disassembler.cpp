#include "disassembler.h"

#include <sstream>

disassembler::disassembler(exe_loader *own)
{
	owner = own;
}

disassembler::~disassembler()
{
}

std::ostream& operator<< (std::ostream& out, instr &a)
{
	if (a.statements.size() == 0)
	{
		out << a.comment << " (0x" << std::hex << a.addr << std::dec << ")";
		if (a.trace_call)
		{
			out << " [trace call] ";
		}
		else if (a.trace_jump)
		{
			out << " [trace jump] ";
		}
	}
	for (unsigned int i = 0; i < a.statements.size(); i++)
	{
		out << a.statements[i];
		if (i == 0)
		{
			out << a.comment << " (0x" << std::hex << a.addr << std::dec << ")";
		}
		if (a.trace_call)
		{
			out << " [trace call] ";
		}
		else if (a.trace_jump)
		{
			out << " [trace jump] ";
		}
		if ((i+1) < a.statements.size())
			out << "\n";
	}
	return out;
}
