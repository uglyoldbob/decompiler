#include "disassembler.h"
#include "helpers.h"

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
		out << a.preprint;
		out << a.comment << " (0x" << std::hex << a.addr << std::dec << ")";
		if (a.trace_call != 0)
		{
			out << " [trace call " << *a.trace_call << "] ";
		}
		else if (a.trace_jump != 0)
		{
			out << " [trace jump " << *a.trace_jump << "] ";
		}
	}
	for (unsigned int i = 0; i < a.statements.size(); i++)
	{
		out << a.preprint;
		out << *a.statements[i] << ";";
		if (i == 0)
		{
			out << tabs(1) << a.comment << " (0x" << std::hex << a.addr << std::dec << ")";
		}
		if (a.trace_call != 0)
		{
			out << " [trace call " << *a.trace_call << "] ";
		}
		else if (a.trace_jump != 0)
		{
			out << " [trace jump " << *a.trace_jump << "] ";
		}
		if ((i+1) < a.statements.size())
			out << "\n";
	}
	return out;
}
