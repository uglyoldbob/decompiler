#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

struct line_info
{
	void *addr;
	char opcode[10];
	char options[300];
	char comment[300];
	int ins;	//increase when control is branched to here
	int is_cbranch;	//set when it is a conditional branch statement
	void *destaddra;	//stores the dest of the conditional branch statement
	void *destaddrb;	//stores the other dest
	int line_num;	//makes it easier for a code block to find the first line of the next block
};

struct code_block
{	//a code block starts in multiple cases
	//the first line of code
	//after any branch instruction
	void *s;	//the starting address
	struct line_info **in;
	int num_lines;
	struct code_block *a;	//0 means last block
	struct code_block *b;	//0 means there is not one
};

//if(){}, else if(){}, else if(){}, else if(){}, else{}
//while () {}
//do {} while ()
//for (;;){}
//switch(){;;;;;;}

class code_element
{
	public:
		code_element();
		virtual ~code_element();
		
		void ss(void *ss);	//sets s
		void *gets();		//gets s
		void sa(code_element *aa);	//sets a
		code_element *ga();
		void sb(code_element *bb);	//sets b
		code_element *gb();
		virtual void fprint(FILE *dest, int depth);
		void setbnum(int n);
		int getbnum();
		int gins();	//get ins
		int is_cbranch();	//does this element have a conditional branch at the end
	protected:
		int ins;	//number of elements that point to this one
		void *s;	//the starting address
		code_element *a;	//next element
		code_element *b;	//other next element (if a decision has to be made)
		int block_num;
};

code_element::code_element()
{
	ins = 0;
	s = NULL;
	a = NULL;
	b = NULL;
}

code_element::~code_element()
{
}

int code_element::is_cbranch()
{	//does this element have a conditional branch at the end
	if ((a != 0) && (b != 0))
		return 1;
	else
		return 0;
}

int code_element::gins()	//get ins
{
	return ins;
}

code_element *code_element::ga()
{
	if (a == 0)
		return b;
	else
		return a;
}

code_element *code_element::gb()
{
	return b;
}

void code_element::fprint(FILE *dest, int depth)
{
	printf("Dummy print\n");
}

void code_element::ss(void* ss)
{
	s = ss;
}

void *code_element::gets()
{
	return s;
}

void code_element::sa(code_element *aa)
{
	a = aa;
}

void code_element::sb(code_element *bb)
{
	b = bb;
}

void code_element::setbnum(int n)
{
	block_num = n;
}

int code_element::getbnum()
{
	return block_num;
}

class code_multi_if : public code_element
{
	public:
		code_multi_if();
		~code_multi_if();
	private:
};

class code_if_else : public code_element
{
	public:
		code_if_else();
		~code_if_else();
		void fprint(FILE *dest, int depth);
		void add_lcb(code_element *add);
		void add_ecb(code_element *add);
		void set_else(code_element *h);
		void set_last(code_element *l);
	private:
		std::vector<code_element*>lcb;	//the logic blocks
		std::vector<code_element*>ecb;	//the executing blocks
		code_element *helse;	//the else block
		code_element *last;	//the block everything merges to
};

code_if_else::code_if_else()
{
	helse = NULL;
	last = NULL;
}

code_if_else::~code_if_else()
{
}

void code_if_else::add_lcb(code_element *add)
{
	lcb.push_back(add);
	if (lcb.size() == 1)
	{
		ins = add->gins();
		s = add->gets();
	}
}

void code_if_else::add_ecb(code_element *add)
{
	ecb.push_back(add);
}

void code_if_else::set_else(code_element *h)
{
	helse = h;
}

void code_if_else::set_last(code_element *l)
{
	last = l;
	a = l->ga();
	b = l->gb();
}

void code_if_else::fprint(FILE *dest, int depth)
{
	int i;
	int xyz;
	lcb[0]->fprint(dest, depth);
	for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
	fprintf(dest, "if ()\n");
	for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
	fprintf(dest, "{\n");
	ecb[0]->fprint(dest, depth+1);
	for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
	fprintf(dest, "}\n");
	for (i = 1; i < lcb.size(); i++)
	{
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "else if (?)\n{");
		ecb[i]->fprint(dest, depth+1);
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "}\n");
	}
	if (helse != 0)
	{
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "else\n");
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "{\n");
		helse->fprint(dest, depth+1);
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "}\n");
	}
	last->fprint(dest, depth);
}

class ce_block : public code_element
{
	public:
		ce_block();
		~ce_block();
		void setnline(int num);
		int getnline();
		void setline(line_info *a);
		line_info *getline(int num);	//-1 means get last line
		void fprint(FILE *dest, int depth);
	private:
		struct line_info **line;
		int num_lines;
};

ce_block::ce_block()
{
	num_lines = 0;
	line = NULL;
}

ce_block::~ce_block()
{
	if (line != NULL)
		delete [] line;
}

void ce_block::fprint(FILE *dest, int depth)
{
	int i;
	for (i = 0; i < depth; i++) { fprintf(dest, "\t");}
	fprintf(dest, "/------");
	if (depth == 0)
		fprintf(dest, "%x (%d input)", s, line[0]->ins);
	fprintf(dest, "\n");	
	int k;
	for (k = 0; k < num_lines; k++)
	{
		for (i = 0; i < depth; i++) { fprintf(dest, "\t");}
		fprintf(dest, "|%x %s %s %s\n", line[k]->addr, line[k]->opcode, line[k]->options, line[k]->comment);
	}
	for (i = 0; i < depth; i++) { fprintf(dest, "\t");}
	fprintf(dest, "\\------ ");
	if (depth == 0)
	{
		if (a != 0)
			fprintf(dest, "%x ", a->gets());
		if (b != 0)
			fprintf(dest, "%x ", b->gets());
	}
	fprintf(dest, "\n");
}

void ce_block::setnline(int num)
{
	if (num_lines == 0)
	{
		num_lines = num;
		line = new line_info*[num_lines];
	}
}

int ce_block::getnline()
{
	return num_lines;
}

void ce_block::setline(line_info *a)
{
	int i;
	for (i = 0; i < num_lines; i++)
	{
		line[i] = &a[i];
	}
	ins = line[0]->ins;
}

line_info *ce_block::getline(int num)
{	//-1 means get last line
	if (num_lines == 0)
	{
		return NULL;
	}
	else if (num < -1)
	{
		return NULL;
	}
	else if (num == -1)
	{
		return line[num_lines-1];
	}
	else if (num < num_lines)
	{
		return line[num];
	}
}

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

class function
{
	public:
		function();
		~function();
		int setio(char *in, char *out);
		void use_input_otool_ppc();
		void compute_branching_ppc();
		void create_blocks();
		void create_pieces();
		void simplify();
		void fprint();
	private:
		FILE *input, *output;
		struct line_info *da_lines;	//all the lines of code for the function (delete these when done)
		int num_lines;
		int actual_num_blocks;
		ce_block *c_blocks;	//the basic blocks of instructions (delete these when done)
		std::vector<code_element *> xblocks; //extra blocks created to simplify logic (delete these when done)
		std::vector<code_element *> pieces;
		void remove_piece(code_element *rmv);	//removes a piece
		void fix_pieces();
		void replace_references(code_element *old, code_element *nw);
		
		int find_if_else();
		int do_simple_if(code_element *a, code_element *b, int i);
		int find_loop();
		int find_runs();
};

function::function()
{
	num_lines = 0;
}

function::~function()
{
	int i;
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
	int i;
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
	int i;
	for (i = 0; i < pieces.size(); i++)
		pieces[i]->setbnum(i);
}

void function::replace_references(code_element *old, code_element *nw)
{
	int i;
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
		int k;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
							int j;
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
						printf(" %d (ok) \n", lcb.back()->ga());
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