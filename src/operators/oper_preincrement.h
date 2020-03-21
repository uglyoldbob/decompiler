#ifndef __OPER_PREINCREMENT_H__
#define __OPER_PREINCREMENT_H__

#include "oper1.h"

//level 3 in operator precedence
//evaluated right to left
class oper_preincrement : public oper1
{
	public:
		oper_preincrement(statement *a);
	private:
		virtual std::ostream &print(std::ostream &out);
};

#endif
