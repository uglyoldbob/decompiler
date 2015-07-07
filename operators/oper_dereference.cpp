#include "oper_dereference.h"

#include "code_elements/code_element.h"
#include "helpers.h"

oper_dereference::oper_dereference(variable *a, variable *b)
	: oper2(a, b)
{
	p = OPER_LVL3;
}

std::ostream &oper_dereference::print(std::ostream &out)
{
	if (this->get_p() < arg1->get_p())
	{	//parentheses required to have correct order
		out << "*(" << *arg1 << ")";
	}
	else
	{
		out << "*" << *arg1;
	}
	return out;
}

bool oper_dereference::needs_trace()
{
	return true;
}

variable* oper_dereference::trace(int d, variable *trc, code_element *cel, int stmt, int line)
{
	variable *ret = 0;
	if (arg1->needs_trace())
	{
		std::cout << tabs(d) << "Tracing memref (" << *trc << ") (" << *arg1 << ")\n";
		ret = cel->trace_prev(d+1, arg1, stmt, line);
	}
	else
	{
		std::cout << tabs(d) << "Cannot trace memory reference " << *arg1 << std::endl;
	}
	if (ret != 0)
	{
		std::cout << tabs(d) << "Traced memref (" << *trc << ") to " << *ret << "\n";
	}
	else
	{
		std::cout << tabs(d) << "Failed to trace memref (" << *trc << ")\n";
	}
	return ret;
}
