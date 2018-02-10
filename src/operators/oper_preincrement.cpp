#include "oper_preincrement.h"

oper_preincrement::oper_preincrement(variable *a)
	: oper1(a)
{
	arg = a;
	p = OPER_LVL3;
}

std::ostream &oper_preincrement::print(std::ostream &out)
{
	if (this->get_p() < arg->get_p())
	{	//parentheses required to have correct order
		out << "++(" << *arg << ")";
	}
	else
	{
		out << "++" << *arg;
	}
	return out;
}

