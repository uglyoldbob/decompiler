#ifndef __OPER_ADD_H__
#define __OPER_ADD_H__

#include "oper2.h"

//level 6 on precedence
//left to right evaluation
class oper_add : public oper2
{
	public:
		oper_add(statement *a, statement *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif
