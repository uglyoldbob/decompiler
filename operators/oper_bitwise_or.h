#ifndef __OPER_BITWISE_OR_H__
#define __OPER_BITWISE_OR_H__

#include "oper2.h"

//level 6 on precedence
//left to right evaluation
class oper_bitwise_or : public oper2
{
	public:
		oper_bitwise_or(variable *a, variable *b);
		virtual variable* trace(int d, variable *trc, code_element *cel, int stmt, int line);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif