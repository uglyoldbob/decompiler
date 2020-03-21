#ifndef __PROJECT_H__
#define __PROJECT_H__

#include "executable/executable.h"
#include <vector>
#include "build_system.h"

/// Basic object for decompilation.
/** Used to contain all objects related to decompilation. */
class project
{
	public:
		project(build_system *bs); ///< Create a project using the specified build system
		void add_program(executable *t); ///< Add the specified executable object to the project
		void set_output_dir(const char *n); ///< Specify the location of the output directory, which contains all generated source code and build system files.
		void write_build_system(); ///< Output all of the build system files to the output directory.
		void write_sources(); ///< Output all of the generates source code to the output directory.
	private:
		build_system *bld; ///< The build system for the project
		std::vector<executable *> exes; ///< The list of executable objects in the project
		std::string dir; ///< The directory where the project is written
};

#endif
