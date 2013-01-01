#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "ce_block.h"
#include "code_element.h"
#include "code_if_else.h"
#include "code_multi_if.h"
#include "config.h"
#include "exceptions.h"
#include "executable.h"
#include "function.h"

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
	executable program;
	int retval = 0;
	try
	{
		if (argc < 2)
		{
			program.load(argv[0]);
		}
		else if (argc >= 2)
		{
			program.load(argv[1]);
		}
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
