#ifndef __EXECUTABLE_H__
#define __EXECUTABLE_H__

#include <stdint.h>
#include <stdio.h>
#include <vector>

#include "exe_loader.h"
#include "function.h"

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
		int load(char *bin_name);
	private:
		int check_pe(FILE *me);
		int rbo;	//used to signal reversed byte order
		FILE *m;
		EXECUTABLE_TYPE exe_type;
		exe_loader *exe_object;
		
		//TODO: create objects to break up code into multiple source files
		std::vector<function*> funcs;	//all the functions of the program
		
		void handle_function(int i);
};

#endif