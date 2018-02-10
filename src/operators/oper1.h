#ifndef __OPER1_H__
#define __OPER1_H__

#include "variable.h"

//all operations can be represented as a single variable
class oper1 : public variable
{
	public:
		oper1(variable *a);
		virtual ~oper1();
	protected:
		variable *arg;	//the only argument for the operator
};

#endif
