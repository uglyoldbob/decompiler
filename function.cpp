#include "function.h"

#include <string>

#include "code_if_else.h"
#include "code_do_while_loop.h"
#include "code_multi_if.h"
#include "code_while_loop.h"
#include "code_run.h"
#include "ce_block.h"
#include "exceptions.h"

function::function(address addr, const char *n, disassembler &disas)
	: name(n)
{
	s = addr;
	std::cout << "Function " << name
			  << " at address 0x" << std::hex << s << std::dec << "\n";
	gather_instructions(disas);
	check_dests();
	std::cout << "Done with function " << name << " ?\n";
}

function::function(address addr, disassembler &disas)
{
	function(addr, "unknown", disas);
}

function::~function()
{
	unsigned int i;
	for (i = 0; i < c_blocks.size(); i++)
		delete c_blocks[i];
	for (i = 0; i < da_lines.size(); i++)
		delete da_lines[i];
	for (i = 0; i < xblocks.size(); i++)
		delete xblocks[i];
}

address function::gets()
{
	return s;
}

std::vector<address> function::get_calls()	//get a list of addresses called as functions
{
	std::vector<address> temp;
	for (unsigned int i = 0; i < da_lines.size(); i++)
	{
		if (da_lines[i]->call != 0)
			temp.push_back(da_lines[i]->call);
	}
	return temp;
}

void function::gather_instructions(disassembler &disas)
{
	std::vector<address> blocks;	//indicates the starting point of blocks
	blocks.push_back(s);
	address offset = 0;
	while (blocks.size() > 0)
	{
		try
		{
			ce_block * tblock = work_on_block(blocks[0]);
			instr *temp;
			int len;
			len = disas.get_instruction(temp, blocks[0]+offset);	//gather an instruction
			tblock->add_line(temp);
			add_line(temp);
			offset += len;
			//if the next instruction does not belong to this block, move to another block
			if (temp->trace_jump)
			{
				std::cout << "Must trace a jump destination\n";
			}
			if ( (temp->destaddra != 0) && (temp->destaddrb != 0) )
			{
				blocks.push_back(temp->destaddra);
				blocks.push_back(temp->destaddrb);
				tblock->done();
				blocks.erase(blocks.begin());
				offset = 0;
			}
			else if ( (temp->destaddra == 0) && (temp->destaddrb == 0) )
			{
				tblock->done();
				blocks.erase(blocks.begin());
				offset = 0;
			}
		}
		catch (block_already_done &e)
		{	//don't work on the block if it is already done
			std::cout << "Block already done\n";
			blocks.erase(blocks.begin());
		}
	}
}

void function::increase_ins(address addr)
{
	for (unsigned int i = 0; i < da_lines.size(); i++)
	{
		if (da_lines[i]->addr == addr)
		{
			da_lines[i]->ins++;
			break;
		}
	}
}

void function::check_dests()
{
	for (unsigned int i = 0; i < da_lines.size(); i++)
	{
		da_lines[i]->ins = 0;
	}
	da_lines[0]->ins = 1;
	for (unsigned int i = 0; i < da_lines.size(); i++)
	{
		if (da_lines[i]->is_cbranch == 1)
		{
			increase_ins(da_lines[i]->destaddra);
			increase_ins(da_lines[i]->destaddrb);
		}
	}

	int num_blocks = 0;
	for (unsigned int i = 0; i < da_lines.size(); i++)
	{
		if (da_lines[i]->ins > 0)
		{
			ce_block *temp;
			temp = new ce_block;
			c_blocks.push_back(temp);
			num_blocks++;
		}
	}
	std::cout << "There are " << num_blocks << " blocks present in this function\n";
}

ce_block *function::work_on_block(address addr)
{	//start working on a block that start at addr
	for (unsigned int i = 0; i < c_blocks.size(); i++)
	{
		try
		{
			if (c_blocks[i]->work(addr) == 1)
			{
				return c_blocks[i];
			}
		}
		catch (block_already_done &e)
		{
			throw;	//pass the exception up
		}
		catch (block_should_be_split &e)
		{	//split up the completed block
			std::cout << "Split an existing block at 0x" << std::hex << addr << std::dec << "\n";
			ce_block *a;
			a = new ce_block(*c_blocks[i], c_blocks[i]->getline(0)->addr, e.what());
			ce_block *b;
			b = new ce_block(*c_blocks[i], e.what(), c_blocks[i]->getline(-1)->addr);
			c_blocks[i] = a;
			c_blocks.insert(c_blocks.begin() + i + 1, b);
			throw block_already_done(addr);
		}
	}
	return new ce_block;
}

void function::add_line(instr *addme)
{
	da_lines.push_back(addme);
}

void function::set_name(const char *to)
{
	name = to;
}

std::string function::get_name()
{
	return name;
}

void function::remove_piece(code_element *rmv)
{
	unsigned int i;
	int removed = 0;
	for (i = 0; i < pieces.size(); i++)
	{
		if (pieces[i] == rmv)
		{
			removed++;
			pieces.erase(pieces.begin() + i);
		}
	}
}

void function::replace_references(code_element *old, code_element *nw)
{
	unsigned int i;
	for (i = 0; i < pieces.size(); i++)
	{
		if (pieces[i] == old)
			pieces[i] = nw;
		pieces[i]->replace_references(old, nw);
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
	} while (reduced > 0);
	if (pieces.size() > 1)
	{
		std::cout << "Failed to simplify enough (" << pieces.size() << "/" << c_blocks.size() << ")\n";
	}
	else
	{
		std::cout << "Logic fully reduced\n";
	}
}

inline std::ostream& operator << (std::ostream& output, function &me)
{
	me.fprint(output);
	return output;
}

void function::fprint(std::ostream &output)
{	//print the code to the output for examination
	unsigned int i;
	output << "//There are " << pieces.size() << " blocks\n";
	output << "int " << name << "(int argc, char *argv[])\n{\n";
	for (i = 0; i < pieces.size(); i++)
	{
		output << "\t****~~~~ " << (int)pieces[i]->gets() << " " << pieces[i]->gins() << " inputs ";
		if (pieces[i]->ga() != 0)
			output << std::hex << (int)pieces[i]->ga()->gets() << std::dec
					<< pieces[i]->ga()->gins();
		if (pieces[i]->gb() != 0)
			output << std::hex << (int)pieces[i]->gb()->gets() << std::dec
					<< pieces[i]->gb()->gins();
		output << "\n";
		pieces[i]->fprint(output, 1);
		output << "\t~~~~**** \n";
	}
	output << "}\n";
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
		if ( (pieces[i]->ga() == pieces[i]) || (pieces[i]->gb() == pieces[i]) )
		{
			code_do_while_loop *temp;
			temp = new code_do_while_loop(pieces[i]);
			xblocks.push_back(temp);
			replace_references(pieces[i], temp);
			found++;
		}
		else if ( (pieces[i]->is_cbranch() != 1) &&
				  (pieces[i]->ga() != 0) &&
				  (pieces[i]->gins() == 1) &&
				  ( (pieces[i]->ga()->ga() == pieces[i]) ||
					(pieces[i]->ga()->gb() == pieces[i])  ) )
		{
			code_while_loop *temp;
			temp = new code_while_loop(pieces[i]->ga(), pieces[i]);
			xblocks.push_back(temp);
			replace_references(pieces[i]->ga(), temp);
			remove_piece(pieces[i]);
			found++;
		}
	}
	return 0;//found;
}

int function::do_simple_if(code_element *a, code_element *b, int i)
{
	if ( (a->gins() == 1) &&
		 (b->gins() >= 2) &&
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
				if (list.back()->is_cbranch() != 1)
				{	//possibly a combination of AND without an ELSE block
					if ( (common->is_cbranch() != 1) &&
						 ( (common->ga() == list.back()->ga()) || (common->ga() == list.back()->gb()) ) )
					{//AND without an ELSE block
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
							//common->ga()->dins(1);
						}
						else if (common->ga()->gins() == 1)
						{
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
							//common->ga()->dins(1);
						}
						else if (common->ga()->gins() == 1)
						{
							remove_piece(common->ga());
						}
						replace_references(pieces[i], temp);
						xblocks.push_back(temp);
						found++;
					}
					else if ( (common->is_cbranch() != 1) && (list.back()->is_cbranch() == 1) &&
							(list.back()->gb()->ga() == list.back()->ga()->ga()) &&
							(common != list.back()->ga()) )
					{	//AND or OR with an ELSE block
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
						temp->set_else(list.back()->ga());
						temp->set_final(common->ga());
						temp->finish_with_else();
						remove_piece(common);
						remove_piece(list.back()->ga());
						if (common->ga()->gins() > 1)
						{
							//common->ga()->dins(1);
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
	return found;
}

int function::do_if_else(int i)
{
	int done = 0;
	int found = 0;
	std::vector<code_element*>lcb;	//logic control blocks
	std::vector<code_element*>ecb;	//executing control blocks
	code_element *end_b = 0;	//end block
	code_element *helse = 0;	//the executing block for the else statement
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
					//helse->ga()->dins(ecb.size());
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
			if (lcb.back()->gb()->is_cbranch() != 1)
			{
				if (lcb.back()->gb()->ga() == lcb.back()->ga())
				{
					ecb.push_back(lcb.back()->gb());
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
					ecb.push_back(lcb.back()->ga());
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
