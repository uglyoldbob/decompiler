#include "oper_bitwise_or.h"

oper_bitwise_or::oper_bitwise_or(variable *a, variable *b)
	: oper2(a, b)
{
	p = OPER_LVL6;
}

std::ostream &oper_bitwise_or::print(std::ostream &out)
{
	if (arg1->get_p() < this->get_p())
	{
		out << "(" << *arg1 << ") + ";
	}
	else
	{
		out << *arg1 << " | ";
	}
	if (arg2->get_p() < this->get_p())
	{
		out << "(" << *arg2 << ")";
	}
	else
	{
		out << *arg2;
	}
	return out;
}

