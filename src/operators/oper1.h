#ifndef __OPER1_H__
#define __OPER1_H__

#include "statement.h"

//all operations can be represented as a single variable
class oper1 : public statement
{
	public:
		oper1(statement *a);
		virtual ~oper1();
	protected:
		statement *arg;	//the only argument for the operator
};

#endif
