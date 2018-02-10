#include "oper_right_shift.h"

oper_right_shift::oper_right_shift(variable *a, variable *b)
	: oper2(a, b)
{
	p = OPER_LVL6;
}

std::ostream &oper_right_shift::print(std::ostream &out)
{
	if (this->get_p() < arg1->get_p())
	{
		out << "(" << *arg1 << ") - ";
	}
	else
	{
		out << *arg1 << ">>";
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

