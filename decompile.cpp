#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "ce_block.h"
#include "code_element.h"
#include "code_if_else.h"
#include "function.h"

class code_multi_if : public code_element
{
	public:
		code_multi_if();
		~code_multi_if();
	private:
};

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Not enough arguments");
		return -1;
	}
	
	function analyze;

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