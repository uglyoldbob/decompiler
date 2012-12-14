#include "function.h"

#include <string.h>

#include "code_if_else.h"
#include "code_do_while_loop.h"
#include "code_multi_if.h"
#include "code_run.h"
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
//	printf("Remove element %x....\n", rmv->gets());
	unsigned int i;
	int removed = 0;
	for (i = 0; i < pieces.size(); i++)
	{
		if (pieces[i] == rmv)
		{
//			printf(" removing %x", pieces[i]->gets());
			removed++;
			pieces.erase(pieces.begin() + i);
		}
	}
//	printf("removed %d elements\n", removed);
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
		if (pieces[i] == old)
		{
			pieces[i] = nw;
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
		sscanf(single_line, "%x\t%s%s%[^\n]", 
			   (int*)&da_lines[i].addr, da_lines[i].opcode, 
			   da_lines[i].options, da_lines[i].comment);
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
				sscanf(da_lines[i].options, "0x%x", (int*)&dest);
				da_lines[i].destaddra = dest;
				da_lines[i].destaddrb = NULL;
				increase_dest_count(da_lines, num_lines, dest);
				da_lines[i].is_cbranch = -1;
			}
			else if (strcmp(da_lines[i].opcode, "bcl") == 0)
			{
				int a, b;
				sscanf(da_lines[i].options, "%d,%d,0x%x", &a, &b, (int*)&dest);
				da_lines[i].destaddra = dest;
				if ((i+1) < num_lines)
				{
					da_lines[i].destaddrb = da_lines[i+1].addr;
				}
				else
				{
					da_lines[i].destaddrb = NULL;
				}
				if (da_lines[i].destaddra != da_lines[i].destaddrb)
				{
					increase_dest_count(da_lines, num_lines, dest);
					if ((i+1) < num_lines)
						increase_dest_count(da_lines, num_lines, da_lines[i+1].addr);
					da_lines[i].is_cbranch = 1;
				}
				else
				{
					da_lines[i].destaddra = NULL;
				}
			}
			else
			{
				int dummy;
				da_lines[i].is_cbranch = 1;
				if (strncmp(da_lines[i].options, "cr", 2) == 0)
				{
					sscanf(da_lines[i].options, "cr%d,0x%x", &dummy, (int*)&dest);
				}
				else
				{
					sscanf(da_lines[i].options, "0x%x", (int*)&dest);
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
//	int stage = 0;
	do
	{
		reduced = 0;
		reduced += find_if_else();
		reduced += find_loop();
		reduced += find_runs();
//		find_stuff(c_blocks, actual_num_blocks, 1);
//		printf("End of stage %d %d\n", ++stage, reduced);
	} while (reduced > 0);
}

void function::fprint()
{	//print the code to the output for examination
	unsigned int i;
	fprintf(output, "There are %d blocks\n", pieces.size());
	for (i = 0; i < pieces.size(); i++)
	{
		fprintf(output, "****~~~~ %x %d inputs ", (int)pieces[i]->gets(), pieces[i]->gins());
		if (pieces[i]->ga() != 0)
			fprintf(output, "%x (%d) ", (int)pieces[i]->ga()->gets(), pieces[i]->ga()->gins());
		if (pieces[i]->gb() != 0)
			fprintf(output, "%x (%d) ", (int)pieces[i]->gb()->gets(), pieces[i]->gb()->gins());
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
		if ((pieces[i]->is_cbranch() != 1) && (pieces[i]->ga() != 0))
		{
			if (pieces[i]->ga()->gins() == 1)
			{
				std::vector<code_element*> lisp;
				code_run *temp;
				temp = new code_run;
				unsigned int j;
				lisp.push_back(pieces[i]);
				while ((lisp.back()->is_cbranch() != 1) && (lisp.back()->ga()->gins() == 1))
				{
					lisp.push_back(lisp.back()->ga());
				}
				if (lisp.size() > 1)
				{
					for (j = 0; j < lisp.size(); j++)
					{
						temp->add_element(lisp[j]);
						if (j != 0)
							remove_piece(lisp[j]);
					}
					temp->done();
					replace_references(pieces[i], temp);
					found++;
					printf("Spotted start of run at block %x\n", (int)pieces[i]->gets());
				}
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
			printf("Do while loop at block %x\n", pieces[i]->gets());
			code_do_while_loop *temp;
			temp = new code_do_while_loop(pieces[i]);
			xblocks.push_back(temp);
			replace_references(pieces[i], temp);
			found++;
		}
		else if ( (pieces[i]->is_cbranch() != 1) &&
				  (pieces[i]->ga() != 0) &&
				  ( (pieces[i]->ga()->ga() == pieces[i]) ||
					(pieces[i]->ga()->gb() == pieces[i])  ) )
		{
			printf("While loop at block %x\n", pieces[i]->ga()->gets());
		}
	}
	return 0;//found;
}

int function::do_simple_if(code_element *a, code_element *b, int i)
{
	if ( (a->gins() == 1) &&
		 (b->gins() == 2) &&
	     (a->ga() == b) && 
	     (a->is_cbranch() != 1) )
	{
		code_if_else *temp;
		temp = new code_if_else;
		temp->add_lcb(pieces[i]);
		temp->add_ecb(a);
		if (b->gins() == 2)
		{
			temp->set_last(b);
			remove_piece(b);
		}
		else if (b->gins() > 2)
		{
			temp->set_next(b);
		}
		xblocks.push_back(temp);
		remove_piece(a);
		replace_references(pieces[i], temp);
//		printf("Block %d replaced with if ()\n", 	i);
		return 1;
	}
	return 0;
}

int function::do_multi_if(int i)
{
	code_element *common = NULL;
	std::vector<code_element *>list;	//list of conditionals
	int done = 0;
	int found = 0;
	list.push_back(pieces[i]);
	do
	{
		done = 1;
		if ( (list.back()->ga()->gins() > 1) &&
			(list.back()->gb()->gins() == 1) )
		{
			if (list.back()->gb()->is_cbranch() == 1)
			{
				if ( (common == 0) && 
					 ((list.back()->ga() == list.back()->gb()->ga()) || ((list.back()->ga() == list.back()->gb()->gb()))) )
				{
					common = list.back()->ga();
					done = 0;
				}
				else if ( (common == list.back()->gb()->ga()) || (common == list.back()->gb()->gb()) )
				{	//match
					done = 0;
				}
				if (done == 0)
				{
					list.push_back(list.back()->gb());
				}
			}
		}
		else if ( (list.back()->gb()->gins() > 1) &&
			(list.back()->ga()->gins() == 1) )
		{
			if (list.back()->ga()->is_cbranch() == 1)
			{
				if ( (common == 0) && (list.back()->gb() == list.back()->ga()->ga()) )
				{
					common = list.back()->gb();
					done = 0;
				}
				else if ( (common == list.back()->ga()->ga()) || (common == list.back()->ga()->gb()) )
				{
					done = 0;
				}
				if (done == 0)
				{
					list.push_back(list.back()->ga());
				}
			}
		}
		if (common != 0)
		{
			if (list.size() == (unsigned int)common->gins())
			{
				unsigned int asdf;
				if (list.back()->is_cbranch() != 1)
				{	//possibly a combination of AND without an ELSE block
					printf("Something found %x\n", (int)list.back()->gets());
					if ( (common->is_cbranch() != 1) &&
						 ( (common->ga() == list.back()->ga()) || (common->ga() == list.back()->gb()) ) )
					{//AND without an ELSE block
						printf("Multiple conditions AND-ed with no ELSE statement at %x\n", (int)pieces[i]->gets());
						code_multi_if *temp;
						temp = new code_multi_if;
						unsigned int q = 0;
						for (q = 0; q < list.size(); q++)
						{
							temp->add(list[q]);
							if (q != 0)
								remove_piece(list[q]);
						}
						temp->common(common);
						temp->finish_and_no_else();
						remove_piece(common);
						replace_references(pieces[i], temp);
						xblocks.push_back(temp);
						found++;
					}
				}
				else
				{	//something else
					if ((common->is_cbranch() != 1) &&
						( (common->ga() == list.back()->ga()) || (common->ga() == list.back()->gb()) ) )
					{
						printf("Multiple conditions OR-ed with no else statement at %x\n", (int)pieces[i]->gets());
						code_multi_if *temp;
						temp = new code_multi_if;
						unsigned int q = 0;
						for (q = 0; q < list.size(); q++)
						{
							temp->add(list[q]);
							if (q != 0)
								remove_piece(list[q]);
						}
						temp->common(common);
						temp->set_final(common->ga());
						temp->finish_or_no_else();
						remove_piece(common);
						if (common->ga()->gins() > 1)
						{
							printf("reducing ins of %x\n", common->ga()->gets());
							common->ga()->dins(1);
						}
						else if (common->ga()->gins() == 1)
						{
							printf("removing %x\n", common->ga()->gets());
							remove_piece(common->ga());
						}
						replace_references(pieces[i], temp);
						xblocks.push_back(temp);
						found++;
					}
					else if ( (common->is_cbranch() != 1) && (list.back()->is_cbranch() == 1) &&
							(list.back()->ga()->ga() == list.back()->gb()->ga()) &&
							(common != list.back()->gb()) )
					{	//AND or OR with an ELSE block
						printf("Multiple conditions ANDed or ORed with ELSE block at %x, last %x, common %x\n", 
							   (int)pieces[i]->gets(), (int)list.back()->gets(), (int)common->gets());
						code_multi_if *temp;
						temp = new code_multi_if;
						unsigned int q = 0;
						for (q = 0; q < list.size(); q++)
						{
							temp->add(list[q]);
							if (q != 0)
								remove_piece(list[q]);
						}
						temp->common(common);
						temp->set_else(list.back()->gb());
						temp->set_final(common->ga());
						temp->finish_with_else();
						remove_piece(common);
						remove_piece(list.back()->gb());
						if (common->ga()->gins() > 1)
						{
							common->ga()->dins(1);
						}
						else if (common->ga()->gins() == 1)
						{
							remove_piece(common->ga());
						}
						replace_references(pieces[i], temp);
						xblocks.push_back(temp);
						found++;
					}
				}
			}
		}
	} while ((!done) && (found == 0));
//	printf("end multi-if\n");
	return found;
}

int function::do_if_else(int i)
{
	int done = 0;
	int found = 0;
	std::vector<code_element*>lcb;	//logic control blocks
	std::vector<code_element*>ecb;	//executing control blocks
	code_element *end_b = NULL;	//end block
	code_element *helse = NULL;	//the executing block for the else statement
	lcb.push_back(pieces[i]);
	while (!done)
	{
		done = 1;
		if ( (lcb.back()->ga()->is_cbranch() == 1) &&
				(lcb.back()->gb()->is_cbranch() != 1) )
		{	//a is conditional, b is not
			if ( (lcb.back()->ga()->gb()->gins() == 1) &&
				 (lcb.back()->gb()->gins() == 1) )
			{
				if (end_b == 0)
				{	//nothing to compare to
					end_b = lcb.back()->gb()->ga();
					done = 0;
				}
				else if (end_b == lcb.back()->gb()->ga())
				{	//these should match
					done = 0;
				}
				if (done == 0)
				{
					ecb.push_back(lcb.back()->gb());
					if (end_b != lcb.back()->ga())
					{
						lcb.push_back(lcb.back()->ga());
					}
					else
					{
						done = 1;
					}
				}
			}
		}
		if ( (lcb.back()->gb()->is_cbranch() == 1) &&
					(lcb.back()->ga()->is_cbranch() != 1) )
		{	//b is conditional, a is not
			if ( (lcb.back()->gb()->gb()->gins() == 1) &&
				 (lcb.back()->ga()->gins() == 1) )
			{
				if (end_b == 0)
				{
					end_b = lcb.back()->ga()->ga();
					done = 0;
				}
				else if (end_b == lcb.back()->ga()->ga())
				{	//these should match
					done = 0;
				}
				if (done == 0)
				{
					ecb.push_back(lcb.back()->ga());
					if (lcb.back()->gb() != end_b)
					{
						lcb.push_back(lcb.back()->gb());
					}
					else
					{
						done = 1;
					}
				}
			}
		}
		if ( (lcb.back()->gb()->is_cbranch() != 1) &&
				(lcb.back()->ga()->is_cbranch() != 1)  &&
				(lcb.back()->ga()->ga() == lcb.back()->gb()->ga()) &&
				(done == 1) )
		{	//neither a nor b are conditional
			if ( (lcb.back()->ga()->gins() == 1) &&
					(lcb.back()->gb()->gins() == 1) &&
					(lcb.back()->ga()->ga()->gins() >= (ecb.size()+2)) )
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
				if (helse->ga()->gins() == (ecb.size()+1))
				{
					temp->set_last(helse->ga());
					remove_piece(helse->ga());
				}
				else
				{
					temp->set_next(helse->ga());
					helse->ga()->dins(ecb.size());
				}
				replace_references(pieces[i], temp);
				xblocks.push_back(temp);
				found++;
				done = 1;
			}
		}
		if ( ((lcb.back()->ga()->is_cbranch() != 1) || 
			 (lcb.back()->gb()->is_cbranch() != 1)) &&
			 (done == 1) && 
		     (ecb.size() > 0) )
		{
/*			printf("if with no else at %x?\n", pieces[i]->gets());
			unsigned int q;
			printf("\tLCB(%d):", lcb.size());
			for (q = 0; q < lcb.size(); q++)
			{
				printf(" %x(", lcb[q]->gets());
				if (lcb[q]->ga() != 0)
					printf("%x,", lcb[q]->ga()->gets());
				if (lcb[q]->gb() != 0)
					printf("%x,", lcb[q]->gb()->gets());
				printf(")");
			}
			printf("\n");
			printf("\tECB(%d):", ecb.size());
			for (q = 0; q < ecb.size(); q++)
			{
				printf(" %x(", ecb[q]->gets());
				if (ecb[q]->ga() != 0)
					printf("%x,", ecb[q]->ga()->gets());
				if (ecb[q]->gb() != 0)
					printf("%x,", ecb[q]->gb()->gets());
				printf(")");
			}
			printf("\n\tEND %x\n", end_b->gets());
			if (lcb.back()->ga()->is_cbranch() != 1)
			{
				printf("Maybe still a no else (lcb a) (%x %x)\n",
					lcb.back()->ga()->ga()->gets(), lcb.back()->gb()->gets()); 
				if (lcb.back()->ga()->ga() == lcb.back()->gb())
				{
					printf("Still a no else (lcb a)\n");
				}
			}
*/			if (lcb.back()->gb()->is_cbranch() != 1)
			{
				if (lcb.back()->gb()->ga() == lcb.back()->ga())
				{
					ecb.push_back(lcb.back()->gb());
					printf("Still a no else (lcb b)\n");
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
					if (end_b->gins() == (ecb.size()+1))
					{
						temp->set_last(end_b);
						remove_piece(end_b);
					}
					else
					{
						temp->set_next(end_b);
					}
					replace_references(pieces[i], temp);
					xblocks.push_back(temp);
					found++;
					done = 1;
				}
			}
			else if (lcb.back()->ga()->is_cbranch() != 1)
			{
				if (lcb.back()->ga()->ga() == lcb.back()->gb())
				{
					printf("Still a no else (lcb a)\n");
				}
			}
		}
	}
	return found;
}

int function::find_if_else()
{	//find a basic if element first
	//block a leads to b/c, block b leads to c
	//block b only has 1 input
	unsigned int i;
	int found = 0;
	for (i = 0; i < pieces.size(); i++)
	{
		if ( (pieces[i]->ga() != pieces[i]) &&
			 (pieces[i]->gb() != pieces[i]) &&
			 (pieces[i]->gb() != NULL) &&
		     (pieces[i]->ga() != NULL) )
		{	//potential i found
			found += do_simple_if(pieces[i]->ga(), pieces[i]->gb(), i);
			if (found == 0)
			{
				found += do_simple_if(pieces[i]->gb(), pieces[i]->ga(), i);
			}
			if (found == 0)
			{
				if ( (pieces[i]->ga()->gins() == 1) &&
						(pieces[i]->gb()->gins() == 1))
				{
					found += do_if_else(i);
				}
			}
			if (found == 0)
			{
				if ( (pieces[i]->gb()->gins() > 1) ||
					(pieces[i]->ga()->gins() > 1) )
				{
					found += do_multi_if(i);
				}
			}
		}
		if (found > 0)
			break;
	}
	return found;
}
