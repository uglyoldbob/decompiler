#include "oper1.h"

oper1::oper1(variable *a)
{
	arg = a;
}

oper1::~oper1()
{
	delete arg;
}
