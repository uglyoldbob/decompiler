#ifndef __RELATED_CODE_H__
#define __RELATED_CODE_H__

#include <vector>

#include "disassembler.h"
#include "code_element.h"

class related_code
{
	public:
		related_code();
		address start_address;
		void add_statement(instr *m);	//Adds a statement to this group of related code
		void gather_instructions(disassembler &disas);
		void fprint(std::ostream &dest, int depth);
		void print_graph(std::ostream &dest);
	private:
		std::vector<code_element *> blocks;	//The basic elements of code
};

#endif
