#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "ce_block.h"
#include "code_element.h"
#include "code_if_else.h"
#include "code_multi_if.h"
#include "executable.h"
#include "function.h"

void reverse(unsigned int *in, int rbo)
{
	if (rbo)
	{
		unsigned int temp;
		temp = ( (((*in)&0xFF000000)>>24) |
				 (((*in)&0x00FF0000)>>8) | 
				 (((*in)&0x0000FF00)<<8) |
				 (((*in)&0x000000FF)<<24) );
		*in = temp;
	}
}

void reverse(unsigned short *in, int rbo)
{
	if (rbo)
	{
		unsigned short temp;
		temp = (*in>>8) | ((*in&0xFF)<<8);
		*in = temp;
	}
}

int main(int argc, char *argv[])
{
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