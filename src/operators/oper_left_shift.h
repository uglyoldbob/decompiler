#ifndef __OPER_LEFT_SHIFT_H__
#define __OPER_LEFT_SHIFT_H__

#include "oper2.h"

/// The left shift operator (<<).
/** The left shift operator (<<). Level 6 precedence, evaluated left to right. */
class oper_left_shift : public oper2
{
	public:
		oper_left_shift(statement *a, statement *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif
