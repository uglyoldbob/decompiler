#ifndef __OPER_RIGHT_SHIFT_H__
#define __OPER_RIGHT_SHIFT_H__

#include "oper2.h"

//level 6 on precedence
//left to right evaluation
class oper_right_shift : public oper2
{
	public:
		oper_right_shift(variable *a, variable *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif