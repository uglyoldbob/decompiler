#ifndef __OPER_SUB_H__
#define __OPER_SUB_H__

#include "oper2.h"

/// The subtraction operator (-).
/** The subtraction operator (-). Level 6 precedence, evaluated left to right. */
class oper_sub : public oper2
{
	public:
		oper_sub(statement *a, statement *b);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif
