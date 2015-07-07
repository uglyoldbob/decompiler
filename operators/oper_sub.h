#ifndef __OPER_SUB_H__
#define __OPER_SUB_H__

#include "oper2.h"

//level 6 on precedence
//left to right evaluation
class oper_sub : public oper2
{
	public:
		oper_sub(variable *a, variable *b);
		virtual variable* trace(int d, variable *trc, code_element *cel, int stmt, int line);
	protected:
		virtual std::ostream &print(std::ostream &out);
	private:
};

#endif