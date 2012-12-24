#ifndef __CODE_DO_WHILE_LOOP_H__
#define __CODE_DO_WHILE_LOOP_H__

#include "code_element.h"
#include <iostream>

class code_do_while_loop : public code_element
{
	public:
		code_do_while_loop(code_element *f);
		~code_do_while_loop();
		void fprint(std::ostream &dest, int depth);
	private:
		code_element *theloop;
};

#endif