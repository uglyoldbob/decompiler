#ifndef __PROJECT_H__
#define __PROJECT_H__

#include "executable/executable.h"
#include <vector>
#include "build_system.h"

class project
{
	public:
		project(build_system *bs);
		void add_program(executable *t);
		void set_output_dir(const char *n);
		void write_build_system();
		void write_sources();
	private:
		build_system *bld;	//the build system for the project
		std::vector<executable *> exes;
		std::string dir;	//the directory for the project
};

#endif
