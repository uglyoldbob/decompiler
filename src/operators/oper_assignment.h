#ifndef __OPER_ASSIGNMENT_H__
#define __OPER_ASSIGNMENT_H__

class code_element;
#include "oper2.h"

/// The assignment operator.
/** The assignment operator. Level 6 precedence, evaluated left to right. */
class oper_assignment : public oper2
{
	public:
		oper_assignment(statement *a, statement *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif
