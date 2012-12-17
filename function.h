#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <stdio.h>
#include <vector>

#include "code_element.h"
#include "ce_block.h"

class function
{
	public:
		function();
		~function();
		int setio(char *in, char *out);
		void use_input_otool_ppc();
		void compute_branching_ppc();
		void create_blocks();
		void create_pieces();
		void simplify();
		void fprint();
		void set_name(const char *to);
		const char *get_name();
	private:
		char *name;
		FILE *input, *output;
		struct line_info *da_lines;	//all the lines of code for the function (delete these when done)
		int num_lines;
		int actual_num_blocks;
		ce_block *c_blocks;	//the basic blocks of instructions (delete these when done)
		std::vector<code_element *> xblocks; //extra blocks created to simplify logic (delete these when done)
		std::vector<code_element *> pieces;
		void remove_piece(code_element *rmv);	//removes a piece
		void replace_references(code_element *old, code_element *nw);
		
		int find_if_else();
		int do_simple_if(code_element *a, code_element *b, int i);
		int do_multi_if(int i);
		int do_if_else(int i);
		int find_loop();
		int find_runs();
};

#endif