#ifndef __OPER2_H__
#define __OPER2_H__

#include "variable.h"

//all operations can be represented as a single variable
class oper2 : public variable
{
	public:
		oper2(variable *a, variable *b);
		virtual ~oper2();
	protected:
		variable *arg1;
		variable *arg2;
};

#endif
