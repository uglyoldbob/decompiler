#ifndef __OPER_SEGBASE_H__
#define __OPER_SEGBASE_H__

#include "variable.h"

class oper_segbase : public variable
{
	public:
		oper_segbase(variable *a, variable *b);
		virtual ~oper_segbase();
	protected:
		virtual std::ostream &print(std::ostream &out);
		variable *segment;
		variable *base;
};

#endif
