#include "code_elements/code_element.h"
#include "helpers.h"
#include "oper_assignment.h"

oper_assignment::oper_assignment(variable *a, variable *b)
	: oper2(a,b)
{
	p = OPER_LVL15;
}

variable* oper_assignment::trace(int d, variable *trc, code_element *cel, int stmt, int line)
{
	variable *dmy;
	std::cout << tabs(d) << "Tracing (" << *trc << ") (" << *arg1 << "=" << *arg2 << ")\n";
	dmy = arg1->trace(d+1, trc, cel, stmt, line);
	if (dmy != 0)
	{
		std::cout << tabs(d) << "Assignment trace\n";
		if (arg2->needs_trace())
		{
			variable *temp = arg2->trace(d+1, arg2, cel, stmt, line);
			if (temp != 0)
				std::cout << tabs(d) << "Returning1 (" << *temp << ")\n";
			else
				std::cout << tabs(d) << "Returning1 0\n";
			return temp;
		}
		else
		{
			if (arg2 != 0)
				std::cout << tabs(d) << "Returning2 (" << *arg2 << ")\n";
			else
				std::cout << tabs(d) << "Returning2 0\n";
			return arg2;
		}
	}
	else
	{
		std::cout << tabs(d) << "Non Assignment trace\n";
		dmy = arg2;
		if (dmy->needs_trace())
		{
			dmy = dmy->trace(d+1, trc, cel, stmt, line);
		}
		if (dmy != 0)
			std::cout << tabs(d) << "Returning3 (" << *dmy << ")\n";
		return dmy;
	}
	std::cout << tabs(d) << "No trace\n";
	return 0;
}

std::ostream &oper_assignment::print(std::ostream &out)
{
	if (this->get_p() < arg1->get_p())
	{
		out << "(" << *arg1 << ") = ";
	}
	else
	{
		out << *arg1 << " = ";
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

