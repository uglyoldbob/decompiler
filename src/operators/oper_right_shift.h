#ifndef __OPER_RIGHT_SHIFT_H__
#define __OPER_RIGHT_SHIFT_H__

#include "oper2.h"

/// The right shfit operator (>>).
/** The right shfit operator (>>). Level 6 precedence, evaluated left to right. */
class oper_right_shift : public oper2
{
	public:
		oper_right_shift(statement *a, statement *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif
