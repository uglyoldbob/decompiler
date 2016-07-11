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

