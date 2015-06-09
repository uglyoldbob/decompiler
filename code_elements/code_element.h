#ifndef __CODE_ELEMENT_H__
#define __CODE_ELEMENT_H__

#include "config.h"
#include "disassembler.h"
#include <iostream>
#include <vector>

class code_element
{
	public:
		code_element(address addr);
		code_element();
		virtual ~code_element();
		
		address gets();		//gets s
		void replace_references(code_element *old, code_element *nw);
		code_element *ga();
		code_element *gb();
		virtual void fprint(std::ostream &dest, int depth);
		int gins();	//get ins
		void copy_inputs(code_element *src);
		void add_input(code_element *ref);
		void remove_input(code_element *me);	//remove input element
		int is_cbranch();	//does this element have a conditional branch at the end
	protected:
		std::vector<code_element*>inputs;	//list of blocks that lead to this one
		address s;	//the starting address
		int depth;
		code_element *a;	//next element
		code_element *b;	//other next element (if a decision has to be made)
};

#endif
