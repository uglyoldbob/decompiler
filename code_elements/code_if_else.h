#ifndef __CODE_IF_ELSE_H__
#define __CODE_IF_ELSE_H__

#include <iostream>
#include <vector>

#include "code_element.h"

class code_if_else : public code_element
{
	public:
		code_if_else();
		virtual ~code_if_else();
		void fprint(std::ostream &dest, int depth);
		void add_lcb(code_element *add);
		void add_ecb(code_element *add);
		void set_else(code_element *h);
		void set_last(code_element *l);
		void set_next(code_element *l);	//when the final element cannot be merged into this block
#ifdef PROVE_SIMPLIFY
		virtual void print_graph(std::ostream &dest);
#endif		
	private:
		std::vector<code_element*>lcb;	//the logic blocks
		std::vector<code_element*>ecb;	//the executing blocks
		code_element *helse;	//the else block
		code_element *next;
};

#endif