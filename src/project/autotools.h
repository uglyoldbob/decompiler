#ifndef __AUTOTOOLS_H__
#define __AUTOTOOLS_H__

#include <string>
#include <vector>
#include "build_system.h"
#include "executable/executable.h"

/// An extension of the build_system class that creates a build system using autotools.
/** An extension of the build_system class that creates a build system using autotools. */
class autotools : public build_system
{
	public:
		virtual void write_files(std::string output_folder); ///< Writes all autotools files to the specified folder.
	private:
		void automake_output(std::string fname); ///< Writes the contents of Makefile.am to the specified file
		void other_output(std::string of); ///< Writes all other files to the directory specified
};

#endif
