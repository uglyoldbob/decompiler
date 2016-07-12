#ifndef __CODE_ELEMENT_H__
#define __CODE_ELEMENT_H__

#include "config.h"
#include "disassembler.h"
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
		bool should_be_added(address);
		bool no_end();
		std::vector<address> get_nexts();
		int contains(address addr);

		void replace_references(code_element *old, code_element *nw);
		virtual void fprint(std::ostream &dest, int depth);
		virtual void print_graph(std::ostream &dest);
		
		code_element* split(address addr);
		void add_line(instr *addme);

		code_element *a;	//next element
		code_element *b;	//other next element (if a decision has to be made)
	protected:
		address s;	//the starting address
		bool is_branch;	//used to declare this element branches and is a finished block
		std::vector<instr> lines;
		int depth;
};

#endif
