#include "oper1.h"

oper1::oper1(statement *a)
{
	arg = a;
}

oper1::~oper1()
{
	delete arg;
}
