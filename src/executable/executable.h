#ifndef __EXECUTABLE_H__
#define __EXECUTABLE_H__

#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>

#include "exe_loader.h"
#include "function.h"
#include "project/source_file.h"

/** EXECUTABLE_TYPE, refers to the various types of executables the system can process. */
enum EXECUTABLE_TYPE
{
	EXEC_TYPE_UNKNOWN,
	EXEC_TYPE_PE,
	EXEC_TYPE_ELF,
	EXEC_TYPE_MACHO32,
	EXEC_TYPE_MACHO64
};

/** ARCHITECTURE_TYPE, refers to the architecture of the system the executable runs on. */
enum ARCHITECTURE_TYPE
{
	ARCH_UNKNOWN,
	ARCH_X86,
	ARCH_X64,
	ARCH_PPC
};

/// This class is used to decompile a single executable file and generate source code for it.
/** This class is used to decompile a single executable file and generate source code for it. It handles a variety of executable types. */
class executable
{
	public:
		executable();
		~executable();
		int load(const char *bin_name); ///< Load the executable from the given file.
		int output(const char *fld_name); ///< Create the container directory for the executable source files.
		std::string get_name(); ///< Retrieve the name of the executable.
		void set_name(const char* n); ///< Set the name of the executable program.
		std::vector<source_file*> get_sources(); ///< Retrieve the list of source code files for the program.
		void write_sources(std::string n); ///< Write all source code files to the specified directory.
	private:
		std::ifstream *exe_file; ///< An input stream used to read file contents.
		std::string folder; ///< The folder specified.
		EXECUTABLE_TYPE exe_type; ///< The type of executable represented by this object.
		exe_loader *exe_object; ///< The object responsible for interpreting data from the input stream.
		std::string exe_name; ///< The name of the executable
		
		bool check_func_list(address addr); ///< Returns true if a function exists that starts at the specified address.
		std::vector<source_file*> sources; ///< A list of source code files used to generate the executable.
};

#endif
