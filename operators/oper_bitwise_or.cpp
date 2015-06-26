#include "oper_bitwise_or.h"

#include "code_elements/code_element.h"

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

variable* oper_bitwise_or::trace(variable *trc, code_element *cel, int stmt, int line)
{
	std::cout << "Tracing | " << *arg1 << " and " << *arg2 << std::endl;
	variable *dmy1, *dmy2;
	if (arg1->needs_trace())
	{
		std::cout << "Tracing | " << *arg1 << std::endl;
		dmy1 = cel->trace_prev(arg1, stmt, line);
	}
	else
	{
		dmy1 = arg1;
	}
	if (arg2->needs_trace())
	{
		std::cout << "Tracing | " << *arg2 << std::endl;
		dmy2 = cel->trace_prev(arg2, stmt, line);
	}
	else
	{
		dmy2 = arg2;
	}
	if ( !dmy1->needs_trace() && !dmy2->needs_trace())
	{
		//create new variable by doing the operation
		
	}
	std::cout << "Finished | tracing [" << *dmy1 << "][" << *dmy2 << "]\n";
	return 0;
}
