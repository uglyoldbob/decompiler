#include "oper_bitwise_or.h"

#include <sstream>
#include "code_elements/code_element.h"
#include "helpers.h"

oper_bitwise_or::oper_bitwise_or(variable *a, variable *b)
	: oper2(a, b)
{
	p = OPER_LVL12;
}

std::ostream &oper_bitwise_or::print(std::ostream &out)
{
	if (this->get_p() < arg1->get_p())
	{
		out << "(" << *arg1 << ") + ";
	}
	else
	{
		out << *arg1 << " | ";
	}
	if (this->get_p() < arg2->get_p())
	{
		out << "(" << *arg2 << ")";
	}
	else
	{
		out << *arg2;
	}
	return out;
}

variable* oper_bitwise_or::trace(int d, variable *trc, code_element *cel, int stmt, int line)
{
	std::cout << tabs(d) << "Trace (" << *trc << ") (" << *arg1 << "|" << *arg2 << ")\n";
	if (this == trc)
	{
		std::cout << tabs(d) << "Tracing for actual expression value\n";
		variable *dmy1, *dmy2;
		if (arg1->needs_trace())
		{
			dmy1 = cel->trace_prev(d+1, arg1, stmt, line);
		}
		else
		{
			dmy1 = arg1;
		}
		if (arg2->needs_trace())
		{
			dmy2 = cel->trace_prev(d+1, arg2, stmt, line);
		}
		else
		{
			dmy2 = arg2;
		}
		if ( !dmy1->needs_trace() && !dmy2->needs_trace())
		{
			//create new variable by doing the operation
			std::string opa(dmy1->get_name());
			std::string opb(dmy2->get_name());
	
			switch (std::max(dmy1->get_size(), dmy2->get_size()))
			{
			case 4:
				{
				std::stringstream ss1;
				std::stringstream ss2;
				std::stringstream ss3;
				uint32_t a, b;
				char dummy;
				ss1 << opa;
				ss1 >> dummy >> dummy;
				ss1 >> std::hex >> a;
				ss2 << opb;
				ss2 >> dummy >> dummy;
				ss2 >> std::hex >> b;
				ss3 << "0x" << std::hex << (a|b);
				std::string compute(ss3.str());
				return new variable(compute, 4);
				}
				break;
			default:
				break;
			}
		}
	}
	else
	{
		std::cout << tabs(d) << "Tracing for variable modification\n";
	}
	std::cout << tabs(d) << "No trace\n";
	return 0;
}
