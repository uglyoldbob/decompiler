#ifndef __OPER_LEFT_SHIFT_H__
#define __OPER_LEFT_SHIFT_H__

#include "oper2.h"

//level 6 on precedence
//left to right evaluation
class oper_left_shift : public oper2
{
	public:
		oper_left_shift(statement *a, statement *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif
