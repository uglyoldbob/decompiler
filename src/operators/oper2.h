#ifndef __OPER2_H__
#define __OPER2_H__

#include "statement.h"

/// All operators / statements that have two items.
/** All operators / statements that have two items. Includes things such as 5 + 6 */
class oper2 : public statement
{
	public:
		oper2(statement *a, statement *b);
		virtual ~oper2();
	protected:
		statement *arg1;
		statement *arg2;
};

#endif
