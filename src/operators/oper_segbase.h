#ifndef __OPER_SEGBASE_H__
#define __OPER_SEGBASE_H__

#include "statement.h"

class oper_segbase : public statement
{
	public:
		oper_segbase(statement *a, statement *b);
		virtual ~oper_segbase();
	protected:
		virtual std::ostream &print(std::ostream &out);
		statement *segment;
		statement *base;
};

#endif
