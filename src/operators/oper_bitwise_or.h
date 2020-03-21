#ifndef __OPER_BITWISE_OR_H__
#define __OPER_BITWISE_OR_H__

#include "oper2.h"

/// The bitwise or operator (|).
/** The bitwise or operator (|). Level 6 precedence, evaluated left to right. */
class oper_bitwise_or : public oper2
{
	public:
		oper_bitwise_or(statement *a, statement *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif
