#ifndef __OPER_DEREFERENCE_H__
#define __OPER_DEREFERENCE_H__

#include "oper1.h"

/// The dereference operator (*).
/** the dereference operator (*). Level 3 precedence, evaluated right to left. */
class oper_dereference : public oper1
{
	public:
		oper_dereference(statement *a);
	private:
		virtual std::ostream &print(std::ostream &out);
};

#endif
