#ifndef __EXECUTABLE_H__
#define __EXECUTABLE_H__

#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>

#include "exe_loader.h"
#include "function.h"
#include "project/source_file.h"

enum EXECUTABLE_TYPE
{
	EXEC_TYPE_UNKNOWN,
	EXEC_TYPE_PE,
	EXEC_TYPE_ELF,
	EXEC_TYPE_MACHO32,
	EXEC_TYPE_MACHO64
};

enum ARCHITECTURE_TYPE
{
	ARCH_UNKNOWN,
	ARCH_X86,
	ARCH_X64,
	ARCH_PPC
};

class executable
{
	public:
		executable();
		~executable();
		int load(const char *bin_name);
		int output(const char *fld_name);
		std::string get_name();
		void set_name(const char* n);
		std::vector<source_file*> get_sources();
		void write_sources(std::string n);
	private:
		int rbo;	//used to signal reversed byte order
		std::ifstream *exe_file;
		std::string folder;
		EXECUTABLE_TYPE exe_type;
		exe_loader *exe_object;
		std::string exe_name;
		
		bool check_func_list(address addr);
		std::vector<source_file*> sources;
};

#endif
