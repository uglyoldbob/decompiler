#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <fstream>
#include <iostream>
#include <vector>

#include "code_elements/code_element.h"
#include "config.h"
#include "var/type.h"

class function
{
	public:
		function(address addr, const char *t, const char *n, disassembler &disas);
		~function();
		std::vector<address> get_calls();	//get a list of addresses called as functions
		void simplify();
		void set_name(const char *to);
		void set_type(type t);
		void output_graph_data(std::string fld_name);
		void output_code(std::string fld_name);
		std::string get_name();
		address gets();
		friend std::ostream& operator << (std::ostream& output, function &me);
	private:
		std::string name;
		address s;
		type ret_type;
		std::vector<instr*> da_lines;	//all the lines of code for the function (delete these when done)
		std::vector<code_element *> c_blocks;	//the basic blocks of instructions (delete these when done)
		std::vector<code_element *> xblocks; //extra blocks created to simplify logic (delete these when done)
		std::vector<code_element *> pieces;
		void remove_piece(code_element *rmv);	//removes a piece
		void replace_references(code_element *old, code_element *nw);
		void replace_cblock_references(code_element *old, code_element *nw);

		//constructor helpers
		void gather_instructions(disassembler &disas);
		void create_pieces();
		void add_line(instr *addme);	//add a line of code to the function
		code_element* find_block(address start);
		void add_block(address addr, code_element *ref);	//consider adding a block starting at address addr
		int check_block(code_element *ref);
		void link_blocks();
		
		//used for simplification
		int find_if_else();
		int do_simple_if(code_element *a, code_element *b, int i);
		int do_multi_if(int i);
		int do_if_else(int i);
		int find_loop();
		int find_runs();
		//used for output
		void fprint(std::ostream& output);
};

#endif
