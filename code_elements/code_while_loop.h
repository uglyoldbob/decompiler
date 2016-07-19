#ifndef __CODE_WHILE_LOOP_H__
#define __CODE_WHILE_LOOP_H__

#include "code_element.h"
#include <iostream>

class code_while_loop : public code_element
{
	public:
		code_while_loop(code_element *f, code_element *g);
		~code_while_loop();
		void fprint(std::ostream &dest, int depth);
		virtual void get_calls(std::vector<address> &c);	//get a list of function calls
#ifdef PROVE_SIMPLIFY
		virtual void print_graph(std::ostream &dest);
#endif
	private:
		code_element *condition;
		code_element *theloop;
};

#endif