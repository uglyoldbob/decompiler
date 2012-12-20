#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "ce_block.h"
#include "code_element.h"
#include "code_if_else.h"
#include "code_multi_if.h"
#include "config.h"
#include "executable.h"
#include "function.h"

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
	printf("Targeting 32-bit executables\n");
#elif TARGET64
	printf("Targeting 64-bit executables\n");
#else
#error "Unknown Target"
#endif
	executable program;
	if (argc < 2)
	{
		program.load(argv[0]);
	}
	else if (argc >= 2)
	{
		program.load(argv[1]);
	}
	
	return 0;
	
	function analyze(0, "test");

	if (analyze.setio(argv[1], argv[2]) == -1)
		return -1;

	analyze.use_input_otool_ppc();
	analyze.compute_branching_ppc();
	analyze.create_blocks();
	analyze.create_pieces();

	analyze.simplify();

	analyze.fprint();

	return 0;
}