#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <fstream>
#include <iostream>
#include <vector>

#include "code_elements/related_code.h"
#include "config.h"
#include "var/type.h"

class function
{
	public:
		function(address addr, const char *t, const char *n, disassembler &disas);
		~function();
		void get_calls(std::vector<address> &c);	//get a list of addresses called as functions
		void simplify();
		void set_name(const char *to);
		void set_type(type t);
		void output_graph_data(std::string fld_name);
		std::string get_name();
		address gets();
		friend std::ostream& operator << (std::ostream& output, function &me);
	private:
		std::string name;
		address s;
		type ret_type;
		std::vector<variable> arguments;	//the arguments of the function
		related_code code;	//the code for the function 

		//constructor helpers
		void gather_instructions(disassembler &disas);
		void create_pieces();
		void add_line(instr *addme);	//add a line of code to the function
		void link_blocks();
		
		//used for output
		void fprint(std::ostream& output);
};

#endif
