#include "oper_dereference.h"

#include "code_elements/code_element.h"
#include "helpers.h"

oper_dereference::oper_dereference(statement *a)
	: oper1(a)
{
	p = OPER_LVL3;
}

std::ostream &oper_dereference::print(std::ostream &out)
{
	if (this->get_p() < arg->get_p())
	{	//parentheses required to have correct order
		out << "*(" << *arg << ")";
	}
	else
	{
		out << "*" << *arg;
	}
	return out;
}

