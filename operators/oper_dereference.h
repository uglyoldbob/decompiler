#ifndef __OPER_DEREFERENCE_H__
#define __OPER_DEREFERENCE_H__

#include "oper2.h"

//level 3 in operator precedence
//evaluated right to left
class oper_dereference : public oper2
{
	public:
		oper_dereference(variable *a, variable *b);
		virtual variable* trace(int d, variable *trc, code_element *cel, int stmt, int line);
		virtual bool needs_trace();
	private:
		virtual std::ostream &print(std::ostream &out);
};

#endif
