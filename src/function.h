#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <fstream>
#include <iostream>
#include <vector>

#include "code_elements/related_code.h"
#include "config.h"
#include "var/type.h"

/// Represents a singular function in the decompiled program
/** This object contains everything necessary to write source code for a single function. */
class function
{
	public:
		function(address addr, const char *t, const char *n, std::vector<code_element *> a);  ///< Creates a function starting at address addr, with a return type of t, named n, and the code specified in the vector a
		~function();
		void get_calls(std::vector<address> &c); ///< Get a list of addresses called as functions.
		void simplify(); ///< Call this to reduce the number of elements in the code. When only one element remains, the function can be represented with source code
		void set_name(const char *to); ///< Set the name of the function
		void set_type(type t); ///< Set the return type of the function
		void output_graph_data(std::string fld_name); ///< Output the logic of the code to a file usable by graphviz
		std::string get_name(); ///< Returns the name of the function
		address gets(); ///< Returns the starting address of the function
		friend std::ostream& operator << (std::ostream& output, function &me); ///< Outputs the source code of the function to the given output stream
	private:
		std::string name; ///< The name of the function
		address s; ///< The starting address of the function.
		type ret_type; //< The return type of the function.
		std::vector<statement> arguments;	///< The arguments of the function
		related_code code;	///< The code for the function 

		//used for output
		void fprint(std::ostream& output); ///< Outputs the source code of the function to the given
};

#endif
