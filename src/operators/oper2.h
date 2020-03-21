#ifndef __OPER2_H__
#define __OPER2_H__

#include "statement.h"

//all operations can be represented as a single variable
class oper2 : public statement
{
	public:
		oper2(statement *a, statement *b);
		virtual ~oper2();
	protected:
		statement *arg1;
		statement *arg2;
};

#endif
