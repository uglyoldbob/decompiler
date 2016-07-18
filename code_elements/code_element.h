#ifndef __CODE_ELEMENT_H__
#define __CODE_ELEMENT_H__

#include "config.h"
#include "disassembly/disassembler.h"
#include <iostream>
#include <vector>

#define PROVE_SIMPLIFY 1
#undef PROVE_SIMPLIFY

class code_element
{
	public:
		code_element(address addr);
		code_element(code_element* in, address start);
		code_element();
		virtual ~code_element();
		
		address gets();		//gets s
		virtual std::vector<address> get_nexts();
		bool is_branch();
		bool jumps_to(code_element *m);
		bool branches_to(code_element *m);
		code_element *other_branch(code_element *m);
		
		virtual std::vector<address> get_calls();	//get a list of function calls

		void replace_references(code_element *old, code_element *nw);
		virtual void fprint(std::ostream &dest, int depth);
		virtual void print_graph(std::ostream &dest);
		
		code_element *a;	//next element
		code_element *b;	//other next element (if a decision has to be made)
	protected:
		address s;	//the starting address
};

#endif
