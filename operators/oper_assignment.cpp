#include "code_elements/code_element.h"
#include "oper_assignment.h"

oper_assignment::oper_assignment(variable *a, variable *b)
	: oper2(a,b)
{
	p = OPER_LVL15;
}

variable* oper_assignment::trace(variable *trc, code_element *cel, int stmt, int line)
{
	std::cout << "Tracing = " << *arg1 << " and " << *arg2 << " stmt " << stmt
			  << " line " << line << std::endl;
	variable *dmy;
	dmy = arg1->trace(trc, cel, stmt, line);
	if (dmy != 0)
	{
		if (arg2->needs_trace())
		{
			std::cout << "Need to trace for = [" << *arg2 << "]" << std::endl;
			return arg2->trace(arg2, cel, stmt, line);
		}
		else
		{
			std::cout << "DONT Need to trace for = [" << *arg2 << "]" << std::endl;
			return arg2;
		}
	}
	dmy = arg2->trace(trc, cel, stmt, line);
	if (dmy != 0)
	{
		std::cout << "= 1\n";
		return dmy;
	}
	else
	{
		std::cout << "= 2\n";
	}
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

