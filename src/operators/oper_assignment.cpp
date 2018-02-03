#include "code_elements/code_element.h"
#include "helpers.h"
#include "oper_assignment.h"

oper_assignment::oper_assignment(variable *a, variable *b)
	: oper2(a,b)
{
	p = OPER_LVL15;
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

