#include "function.h"

#include <string.h>

#include "code_if_else.h"
#include "ce_block.h"

void increase_dest_count(struct line_info* array, int num_elements, void* addr)
{
	int i;
	for (i = 0; i < num_elements; i++)
	{
		if (array[i].addr == addr)
		{
			array[i].ins++;
			return;
		}
	}
}

struct ce_block * find_block(ce_block *blocks, int num_blocks, void* addr)
{	//find a block that starts with addr (-1 if not found)
	int k;
	for (k = 0; k < num_blocks; k++)
	{
		if (blocks[k].gets() == addr)
		{
			return &blocks[k];
		}
	}
	return NULL;
}

int find_stuff(struct code_block* c, int num_blocks, int j)
{
	int found = 0;
	int i;
	for (i = 0; i < num_blocks; i++)
	{
		if (c[i].in[0]->ins == j)
		{
//			printf("%d input block %d\n", j, i);
			found++;
		}
	}
	printf("There are %d %d input blocks\n", found, j);
	return found;
}

function::function()
{
	num_lines = 0;
}

function::~function()
{
	unsigned int i;
	if (output != 0)
		fclose(output);
	if (c_blocks != 0)
		delete [] c_blocks;
	if (da_lines != 0)
		delete [] da_lines;
	for (i = 0; i < xblocks.size(); i++)
	{
		delete xblocks[i];
	}
}

void function::remove_piece(code_element *rmv)
{
	unsigned int i;
	for (i = 0; i < pieces.size(); i++)
	{
		if (pieces[i] == rmv)
		{
			pieces.erase(pieces.begin() + i);
		}
	}
	for (i = 0; i < pieces.size(); i++)
	{
		pieces[i]->setbnum(i);
	}
}

void function::fix_pieces()
{
	unsigned int i;
	for (i = 0; i < pieces.size(); i++)
		pieces[i]->setbnum(i);
}

void function::replace_references(code_element *old, code_element *nw)
{
	unsigned int i;
	for (i = 0; i < pieces.size(); i++)
	{
		if (pieces[i]->ga() == old)
		{
			pieces[i]->sa(nw);
		}
		if (pieces[i]->gb() == old)
		{
			pieces[i]->sb(nw);
		}
	}
}

int function::setio(char *in, char *out)
{
	input = fopen(in, "r");
	output = fopen(out, "w");
	if (input == NULL)
	{
		printf("Failed to open %s\n", in);
		return -1;
	}
	if (output == NULL)
	{
		printf("Failed to open %s\n", out);
		return -1;
	}
	return 0;
}

void function::use_input_otool_ppc()
{	//the input is the output of "otool -tV" powerpc
	char single_line[500];
	int i;
//count the number of lines
	while (feof(input) == 0)
	{
		fgets(single_line, 499, input);
		num_lines++;
	}
	fseek(input, 0, SEEK_SET);
	printf("There are %d lines\n", num_lines);

//allocate and initialize memory for all the lines of code
	da_lines = new line_info[num_lines];
	for (i = 0; i < num_lines; i++)
	{
		da_lines[i].ins = 0;
		da_lines[i].is_cbranch = 0;
		da_lines[i].line_num = i;
		da_lines[i].destaddra = NULL;
		da_lines[i].destaddrb = NULL;
	}
	da_lines[0].ins = 1;	//the first block always is the entry point

//read in all of the lines of code
	for (i = 0; i < num_lines; i++)
	{
		fgets(single_line, 499, input);
		sscanf(single_line, "%x\t%s%s%[^\n]", &da_lines[i].addr, da_lines[i].opcode, da_lines[i].options, da_lines[i].comment);
	}
	fclose(input);
}

void function::compute_branching_ppc()
{	//calculate and insert branching data
	int i;
	for (i = 0; i < num_lines; i++)
	{
		void *dest;
		if (da_lines[i].opcode[0] == 'b')
		{
			if (strcmp(da_lines[i].opcode, "bl") == 0)
			{	//another function call
			}
			else if (strcmp(da_lines[i].opcode, "blr") == 0)
			{	//jump to the link register
				da_lines[i].is_cbranch = -1;
			}
			else if (strcmp(da_lines[i].opcode, "bctrl") == 0)
			{	//just a function call
			}
			else if (strcmp(da_lines[i].opcode, "b") == 0)
			{
				sscanf(da_lines[i].options, "0x%x", &dest);
				da_lines[i].destaddra = dest;
				da_lines[i].destaddrb = NULL;
				increase_dest_count(da_lines, num_lines, dest);
				da_lines[i].is_cbranch = -1;
			}
			else
			{
				int dummy;
				da_lines[i].is_cbranch = 1;
				if (strncmp(da_lines[i].options, "cr", 2) == 0)
				{
					sscanf(da_lines[i].options, "cr%d,0x%x", &dummy, &dest);
				}
				else
				{
					sscanf(da_lines[i].options, "0x%x", &dest);
				}
				da_lines[i].destaddra = dest;
				if ((i+1) < num_lines)
				{
					da_lines[i].destaddrb = da_lines[i+1].addr;
				}
				else
				{
					da_lines[i].destaddrb = NULL;
				}
				increase_dest_count(da_lines, num_lines, dest);
				if ((i+1) < num_lines)
					increase_dest_count(da_lines, num_lines, da_lines[i+1].addr);
				da_lines[i].is_cbranch = 1;
			}
		}
	}
}

void function::create_blocks()
{	//use branching data to compute number of blocks
	//this will generate more than the actual number of blocks
	int i;
	int num_blocks = 0;
	for (i = 0; i < num_lines; i++)
	{
		if (da_lines[i].ins > 0)
		{
			num_blocks++;
			if (i > 0)
			{
				if (da_lines[i-1].is_cbranch == 0)
				{	//tidy up the ends of blocks ending without a branch
					da_lines[i].ins++;
					da_lines[i-1].destaddra = da_lines[i].addr;
				}
			}
		}
	}
	printf("There are %d code blocks\n", ++num_blocks);
//allocate memory for code blocks
	c_blocks = new ce_block[num_blocks];

//determine how many blocks there really are
//also perform some initialization of all real code blocks
	int j = 0;
	for (i = 0; i < num_blocks; i++)
	{
		int nlcb = 0;
		int found_end = 0;
		c_blocks[i].ss((void*)da_lines[j].addr);
		c_blocks[i].sa(NULL);
		c_blocks[i].sb(NULL);
		do
		{
			if ( (da_lines[j].ins > 0) && (nlcb > 0) )
			{	//any line with a dest_count besides the first one is in the next block
				found_end = 1;
			}
			else if (da_lines[j].is_cbranch != 0)
			{	//any line with a conditional branch is the last line of a block
				j++;
				nlcb++;
				found_end = 1;
			}
			else if ((j+1) >= num_lines)
			{	//if this is the last line, it is also the last line of a block
				found_end = 1;
				j++;
				nlcb++;
			}
			else
			{	//anything else belongs to the block
				nlcb++;
				j++;
			}
		} while (found_end == 0);
		c_blocks[i].setnline(nlcb);
		if (j >= num_lines)
			break;
	}
	actual_num_blocks = i + 1;
	printf("There were only %d blocks of code\n", actual_num_blocks);

//copy line references to each code block
//set next_block variables for each block
	j = 0;
	for (i = 0; i < actual_num_blocks; i++)
	{
		c_blocks[i].setline(&da_lines[j]);
		c_blocks[i].setbnum(i);
		j += c_blocks[i].getnline();

		c_blocks[i].sa(find_block(c_blocks, actual_num_blocks, 
						c_blocks[i].getline(-1)->destaddra));
		c_blocks[i].sb(find_block(c_blocks, actual_num_blocks, 
						c_blocks[i].getline(-1)->destaddrb));
	}
}

void function::simplify()
{
	int reduced = 0;
	do
	{
		reduced = 0;
		reduced += find_if_else();
		reduced += find_loop();
		reduced += find_runs();
//		find_stuff(c_blocks, actual_num_blocks, 1);
	} while (reduced > 0);
}

void function::fprint()
{	//print the code to the output for examination
	unsigned int i;
	fprintf(output, "There are %d blocks\n", pieces.size());
	for (i = 0; i < pieces.size(); i++)
	{
		fprintf(output, "****~~~~ %x %d inputs ", pieces[i]->gets(), pieces[i]->gins());
		if (pieces[i]->ga() != 0)
			fprintf(output, "%x ", pieces[i]->ga()->gets());
		if (pieces[i]->gb() != 0)
			fprintf(output, "%x ", pieces[i]->gb()->gets());
		fprintf(output, "\n");
		pieces[i]->fprint(output, 0);
		fprintf(output, "~~~~**** \n");
		
	}
}

void function::create_pieces()
{	//create the list of pieces to be examined
	int i;
	for (i = 0; i < actual_num_blocks; i++)
	{
		pieces.push_back(&c_blocks[i]);
	}
}

int function::find_runs()
{
	unsigned int i;
	int found = 0;
	for (i = 0; i < pieces.size(); i++)
	{
		if ((pieces[i]->is_cbranch() == 0) && (pieces[i]->ga() != 0))
		{
			if (pieces[i]->ga()->gins() == 1)
			{
				printf("Spotted start of run at block %d\n", i);
			}
		}
	}
	return found;
}

int function::find_loop()
{
	int found = 0;
	unsigned int i;
	for (i = 0; i < pieces.size(); i++)
	{
		if ((pieces[i]->ga() == pieces[i]) || (pieces[i]->gb() == pieces[i]) )
		{
			printf("Loop at block %d\n", i);
			found++;
		}
	}
	return 0;//found;
}

int function::do_simple_if(code_element *a, code_element *b, int i)
{
	if ( (a->gins() == 1) &&
		 (b->gins() == 2) &&
	     (a->ga() == b) )
	{
		code_if_else *temp;
		temp = new code_if_else;
		temp->add_lcb(pieces[i]);
		temp->add_ecb(a);
		temp->set_last(b);
		xblocks.push_back(temp);
		remove_piece(a);
		remove_piece(b);
		replace_references(pieces[i], temp);
		pieces[i] = temp;
		fix_pieces();
		printf("Block %d replaced with if ()\n", 	i);
		return 1;
	}
	return 0;
}

int function::find_if_else()
{	//find a basic if element first
	//block a leads to b/c, block b leads to c
	//block b only has 1 input
	unsigned int i;
	int found = 0;
	int temp;
	for (i = 0; i < pieces.size(); i++)
	{
		if ( (pieces[i]->ga() != pieces[i]) &&
			 (pieces[i]->gb() != pieces[i]) &&
			 (pieces[i]->gb() != NULL) &&
		     (pieces[i]->ga() != NULL) )
		{	//potential i found
			temp = do_simple_if(pieces[i]->ga(), pieces[i]->gb(), i);
			found += temp;
			if (temp == 0)
			{
				temp = do_simple_if(pieces[i]->gb(), pieces[i]->ga(), i);
				found += temp;
			}
			if ( (pieces[i]->ga()->gins() == 1) &&
					  (pieces[i]->gb()->gins() == 1))
			{
				int done = 0;
				std::vector<code_element*>lcb;	//logic control blocks
				std::vector<code_element*>ecb;	//executing control blocks
				code_element *end_b = NULL;	//end block
				code_element *helse = NULL;	//the executing block for the else statement
				lcb.push_back(pieces[i]);
				while (!done)
				{
					done = 1;
					if ( (lcb.back()->ga()->is_cbranch()) &&
						 (lcb.back()->gb()->is_cbranch() == 0) )
					{	//a is conditional, b is not
						if ( (lcb.back()->ga()->ga()->gins() == 1) &&
							(lcb.back()->ga()->gb()->gins() == 1) &&
							(lcb.back()->gb()->gins() == 1) )
						{
							lcb.push_back(lcb.back()->ga());
							ecb.push_back(lcb.back()->gb());
							if (end_b == 0)
							{	//nothing to compare to
								end_b = lcb.back()->gb()->ga();
								done = 0;
							}
							else if (end_b == lcb.back()->gb()->ga())
							{	//these should match
								done = 0;
							}
						}
					}
					if ( (lcb.back()->gb()->is_cbranch()) &&
							  (lcb.back()->ga()->is_cbranch() == 0) )
					{	//b is conditional, a is not
						if ( (lcb.back()->gb()->ga()->gins() == 1) &&
							(lcb.back()->gb()->gb()->gins() == 1) &&
							(lcb.back()->ga()->gins() == 1) )
						{
							
							lcb.push_back(lcb.back()->gb());
							ecb.push_back(lcb.back()->ga());
							if (end_b == 0)
							{
								end_b = lcb.back()->ga()->ga();
								done = 0;
							}
							else if (end_b == lcb.back()->ga()->ga())
							{	//these should match
								done = 0;
							}
						}
					}
					if ( (lcb.back()->gb()->is_cbranch() == 0) &&
						  (lcb.back()->ga()->is_cbranch() == 0)  &&
						  (lcb.back()->ga()->ga() == lcb.back()->gb()->ga()) )
					{	//neither a nor b are conditional
						if ( (lcb.back()->ga()->gins() == 1) &&
							 (lcb.back()->gb()->gins() == 1) &&
						     (lcb.back()->ga()->ga()->gins() == (ecb.size()+2)) )
						{
							ecb.push_back(lcb.back()->ga());
							helse = lcb.back()->gb();
							code_if_else *temp;
							temp = new code_if_else;
							unsigned int j;
							for (j = 0; j < lcb.size(); j++)
							{
								temp->add_lcb(lcb[j]);
								if (j != 0)
									remove_piece(lcb[j]);
							}
							for (j = 0; j < ecb.size(); j++)
							{
								temp->add_ecb(ecb[j]);
								remove_piece(ecb[j]);
							}
							temp->set_else(helse);
							remove_piece(helse);
							temp->set_last(helse->ga());
							remove_piece(helse->ga());
							replace_references(pieces[i], temp);
							xblocks.push_back(temp);
							pieces[i] = temp;
							found++;
							fix_pieces();
							printf("installed if/else-if/else block\n");
						}
						//end_b.push_back(lcb.back()->ga()->ga());
					}
					if ( (lcb.back()->ga()->gins() == (ecb.size()+2)) &&
						 (lcb.back()->gb()->ga() == end_b)	)
					{
						printf(" %d (ok) \n", lcb.back()->ga()->gets());
					}
					if ( (lcb.back()->gb()->gins() == (ecb.size()+2)) &&
						 (lcb.back()->ga()->ga() == end_b)	)
					{
						printf(" %d (ok) \n", lcb.back()->gb()->getbnum());
					}
				}
			}
		}
	}
	printf("There were %d if/else blocks\n", found);
	return found;
}
