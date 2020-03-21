#ifndef __BUILD_SYSTEM_H__
#define __BUILD_SYSTEM_H__

#include "executable/executable.h"

/// A class for building a system so the generated source code can be compiled.
/** A class for building a system so the generated source code can be compiled. Inherit this class to implement various build systems. */
class build_system
{
	public:
		virtual void write_files(std::string output_folder) = 0; ///< A virtual functino for generating all build system files in the specified directory
		void add_prog(executable *t); ///< Add a program to the build system
	protected:
		std::vector<executable *> progs; ///< A list of programs the build system needs to generate
};

#endif
