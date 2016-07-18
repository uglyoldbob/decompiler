#ifndef __SOURCE_FILE_H__
#define __SOURCE_FILE_H__

#include "function.h"
#include <string>

class source_file
{
	public:
		source_file(std::string n);
		~source_file();
		void add_function(function* f);
		std::string get_name();
		void write_sources(std::string n);
	private:
		std::string name;

		std::vector<function*> funcs;	//all the functions of the program
};

#endif
