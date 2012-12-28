#ifndef __CODE_ELEMENT_H__
#define __CODE_ELEMENT_H__

#include "config.h"
#include "disassembler.h"
#include <iostream>
#include <vector>

void begin_line(std::ostream &b, int a);

struct code_block
{	//a code block starts in multiple cases
	//the first line of code
	//after any branch instruction
	address s;	//the starting address
	std::vector<instr*> lines;
	struct code_block *a;	//0 means last block
	struct code_block *b;	//0 means there is not one
};

class code_element
{
	public:
		code_element();
		virtual ~code_element();
		
		void ss(address ss);	//sets s
		address gets();		//gets s
		void sa(code_element *aa);	//sets a
		code_element *ga();
		void sb(code_element *bb);	//sets b
		code_element *gb();
		virtual void fprint(std::ostream &dest, int depth);
		int gins();	//get ins
		void dins(int by);	//decrease ins
		int is_cbranch();	//does this element have a conditional branch at the end
	protected:
		int ins;	//number of elements that point to this one
		address s;	//the starting address
		code_element *a;	//next element
		code_element *b;	//other next element (if a decision has to be made)
};

#endif
