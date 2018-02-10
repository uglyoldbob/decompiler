#ifndef __CODE_DO_WHILE_LOOP_H__
#define __CODE_DO_WHILE_LOOP_H__

#include "code_element.h"
#include <iostream>

class code_do_while_loop : public code_element
{
	public:
		code_do_while_loop(code_element *f);
		static bool check(code_element *e);
		static code_element *simplify(std::vector<code_element *> grp, code_element *end);
		~code_do_while_loop();
		void fprint(std::ostream &dest, int depth);
		virtual void get_calls(std::vector<address> &c);	//get a list of function calls
#ifdef PROVE_SIMPLIFY
		virtual void print_graph(std::ostream &dest);
#endif
	private:
		code_do_while_loop();
		std::vector<code_element *>theloop;
};

#endif