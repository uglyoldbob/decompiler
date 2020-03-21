#ifndef __SOURCE_FILE_H__
#define __SOURCE_FILE_H__

#include "function.h"
#include <string>

/// An object for describing a single source code file.
/** An object for describing a single source code file. Contains everything necessary to generate source code. */
class source_file
{
	public:
		source_file(std::string n); ///< Create a source code file with the given name. Name must have the appropriate extension.
		~source_file();
		void add_function(function* f); ///< Add a function to this source code file.
		bool find_function(address a); ///< Locate a function in this source code file.
		void get_calls(std::vector<address> &c); ///< Get a list of addresses called as functions in this source code file.
		std::string get_name(); ///< Retrieve the name of the source code file.
		void write_sources(std::string n); ///< Write source code to the output file.
	private:
		std::string name;

		std::vector<function*> funcs;	//all the functions of the program
};

#endif
