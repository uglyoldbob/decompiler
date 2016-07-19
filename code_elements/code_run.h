#ifndef __CODE_RUN_H__
#define __CODE_RUN_H__

#include "code_element.h"
#include <iostream>
#include <vector>

class code_run : public code_element
{
	public:
		code_run();
		~code_run();
		static code_element *simplify(std::vector<code_element *> grp, code_element *end);
		void add_element(code_element *add);
		void done();
		void fprint(std::ostream &dest, int depth);
		virtual void get_calls(std::vector<address> &c);	//get a list of function calls
#ifdef PROVE_SIMPLIFY
		virtual void print_graph(std::ostream &dest);
#endif
	private:
		std::vector<code_element *> els;	//a string of elements
};

#endif
