#include "oper_dereference.h"

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

