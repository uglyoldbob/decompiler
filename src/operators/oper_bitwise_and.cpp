#include "oper_bitwise_and.h"

oper_bitwise_and::oper_bitwise_and(variable *a, variable *b)
	: oper2(a, b)
{
	p = OPER_LVL10;
}

std::ostream &oper_bitwise_and::print(std::ostream &out)
{
	if (this->get_p() < arg1->get_p())
	{
		out << "(" << *arg1 << ") + ";
	}
	else
	{
		out << *arg1 << " & ";
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

