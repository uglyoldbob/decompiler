#ifndef __OPER_BITWISE_OR_H__
#define __OPER_BITWISE_OR_H__

#include "oper2.h"

//level 6 on precedence
//left to right evaluation
class oper_bitwise_or : public oper2
{
	public:
		oper_bitwise_or(statement *a, statement *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif
