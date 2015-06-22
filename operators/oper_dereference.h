#ifndef __OPER_DEREFERENCE_H__
#define __OPER_DEREFERENCE_H__

#include "oper2.h"

//level 3 in operator precedence
//evaluated right to left
class oper_dereference : public oper2
{
	public:
		oper_dereference(variable *a, variable *b);
	private:
		virtual std::ostream &print(std::ostream &out);
};

#endif
