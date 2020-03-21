#ifndef __OPER_BITWISE_AND_H__
#define __OPER_BITWISE_AND_H__

#include "oper2.h"

//level 6 on precedence
//left to right evaluation
/// The bitwise and operator (&).
/** The bitwise and operator (&). Level 6 precedence, evaluated left to right. */
class oper_bitwise_and : public oper2
{
	public:
		oper_bitwise_and(statement *a, statement *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif
