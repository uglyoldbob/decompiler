#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

#include "code_elements/code_element.h"
#include "code_elements/code_if_else.h"
#include "code_elements/code_multi_if.h"
#include "code_elements/related_code.h"
#include "config.h"
#include "exceptions.h"
#include "executable/executable.h"
#include "function.h"
#include "project/autotools.h"
#include "project/project.h"

void reverse(uint64_t *in, int rbo)
{
	if (rbo)
	{
		uint64_t temp;
		temp = ( (((*in)&0xFF00000000000000)>>56) |
				 (((*in)&0x00FF000000000000)>>40) |
				 (((*in)&0x0000FF0000000000)>>24) | 
				 (((*in)&0x000000FF00000000)>>8) |
				 (((*in)&0x00000000FF000000)<<8) |
				 (((*in)&0x0000000000FF0000)<<24) | 
				 (((*in)&0x000000000000FF00)<<40) |
				 (((*in)&0x00000000000000FF)<<56) );
		*in = temp;
	}
}

void reverse(uint32_t *in, int rbo)
{
	if (rbo)
	{
		uint32_t temp;
		temp = ( (((*in)&0xFF000000)>>24) |
				 (((*in)&0x00FF0000)>>8) | 
				 (((*in)&0x0000FF00)<<8) |
				 (((*in)&0x000000FF)<<24) );
		*in = temp;
	}
}

void reverse(uint16_t *in, int rbo)
{
	if (rbo)
	{
		uint16_t temp;
		temp = (*in>>8) | ((*in&0xFF)<<8);
		*in = temp;
	}
}

int main(int argc, char *argv[])
{
#if TARGET32
#elif TARGET64
#else
#error "Unknown Target"
#endif
	std::unique_ptr<project> sysproj;
	std::unique_ptr<build_system> bsys;
	bsys = std::unique_ptr<build_system>(new autotools());
	sysproj = std::unique_ptr<project>(new project(bsys.get()));	
	executable program;
	int retval = 0;
	related_code::list_code_element_makers(std::cout);
	try
	{
		if (argc < 3)
		{
			sysproj->set_output_dir("./default");
			program.output("./default");
		}
		else if (argc >= 3)
		{
			sysproj->set_output_dir(argv[2]);
			program.output(argv[2]);
		}
		if (argc < 2)
		{
			program.set_name(argv[0]);
			program.load(argv[0]);
			sysproj->add_program(&program);
		}
		else if (argc >= 2)
		{
			program.set_name(argv[1]);
			program.load(argv[1]);
			sysproj->add_program(&program);
		}
		sysproj->write_build_system();
		sysproj->write_sources();
	}
	catch (address_not_present &e)
	{
		std::cout << "Address 0x" << std::hex << e.what() << std::dec << " not found in executable\n";
		retval = -1;
	}
	catch (unknown_exe_format &e)
	{
		std::cout << "Unknown exe format " << e.what() << std::endl;
		retval = -1;
	}
	catch (invalid_instruction &e)
	{
		std::cout << "Invalid instruction at 0x" << std::hex << e.what() << std::dec << "\n";
		retval = -1;
	}
	catch (...)
	{
		std::cout << "Unexpected exception\n";
		retval = -1;
	}
	
	return retval;
}
