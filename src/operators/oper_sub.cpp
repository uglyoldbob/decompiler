#include "oper_sub.h"

#include <sstream>
#include "code_elements/code_element.h"
#include "helpers.h"

oper_sub::oper_sub(statement *a, statement *b)
	: oper2(a, b)
{
	p = OPER_LVL6;
}

std::ostream &oper_sub::print(std::ostream &out)
{
	if (this->get_p() < arg1->get_p())
	{
		out << "(" << *arg1 << ") - ";
	}
	else
	{
		out << *arg1 << " - ";
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

