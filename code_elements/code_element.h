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
		void replace_references(code_element *old, code_element *nw);
		void set_a(code_element *nwa);
		void set_b(code_element *nwb);
		code_element *ga();
		int gains();
		code_element *gb();
		int gbins();
		virtual void fprint(std::ostream &dest, int depth);
		virtual void print_graph(std::ostream &dest);
		int gins();	//get ins
		instr *getline(int num);	//-1 means get last line
		int contains(address addr);
		void copy_inputs(code_element *src);
		void add_input(code_element *ref);
		void remove_input(code_element *me);	//remove input element
		int is_cbranch();	//does this element have a conditional branch at the end
		int is_done();
		
		code_element* split(address addr);
		void add_line(instr *addme);
		void done();
	protected:
		std::vector<code_element*>inputs;	//list of blocks that lead to this one
		address s;	//the starting address
		std::vector<instr*> lines;
		int depth;
		code_element *a;	//next element
		code_element *b;	//other next element (if a decision has to be made)
		
		int finished;
};

#endif
