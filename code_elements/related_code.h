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
		void simplify();
	private:
		std::vector<code_element *> blocks;	//The basic elements of code
		void finalize_blocks();
		code_element *get_block(address a);	//return the block starting with address a
		void replace_element(code_element* old, code_element *n);
		int outside_references(std::vector<code_element *> gr);
		int external_inputs(std::vector<code_element *> gr);
		void apply_combination(std::vector<unsigned int> cmb, std::vector<code_element *> &gr);
		bool next_combo(std::vector<unsigned int> &cmb);
		int process_blocks(int n);
};

#endif
