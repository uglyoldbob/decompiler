#ifndef __OPER1_H__
#define __OPER1_H__

#include "statement.h"

/// All operators / statements that only have one item.
/** All operators / statements that only have one item. Includes things such as i++; */
class oper1 : public statement
{
	public:
		oper1(statement *a);
		virtual ~oper1();
	protected:
		statement *arg;	///< The only argument for the operator
};

#endif
