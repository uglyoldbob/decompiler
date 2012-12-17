#ifndef __CODE_WHILE_LOOP_H__
#define __CODE_WHILE_LOOP_H__

#include "code_element.h"

class code_while_loop : public code_element
{
	public:
		code_while_loop(code_element *f, code_element *g);
		~code_while_loop();
		void fprint(FILE *dest, int depth);
	private:
		code_element *condition;
		code_element *theloop;
};

#endif