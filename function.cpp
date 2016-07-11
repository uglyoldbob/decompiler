#include "function.h"

#include <iostream>
#include <fstream>
#include <string>

#include "code_elements/code_if_else.h"
#include "code_elements/code_do_while_loop.h"
#include "code_elements/code_multi_if.h"
#include "code_elements/code_while_loop.h"
#include "code_elements/code_run.h"
#include "code_elements/code_element.h"
#include "exceptions.h"

function::function(address addr, const char *t, const char *n, disassembler &disas)
	: name(n), ret_type(t)
{
	s = addr;
	//std::cout << "Function " << name
	//		  << " at address 0x" << std::hex << s << std::dec << "\n";
	gather_instructions(disas);
	create_pieces();
	link_blocks();
	//fprint(std::cout);
	simplify();
//	std::cout << "Done with function " << name << " ?\n";
	//std::cout << *this << std::endl;
}

function::~function()
{
	unsigned int i;
	for (i = 0; i < c_blocks.size(); i++)
		delete c_blocks[i];
	c_blocks.clear();
	for (i = 0; i < da_lines.size(); i++)
		delete da_lines[i];
	da_lines.clear();
	for (i = 0; i < xblocks.size(); i++)
		delete xblocks[i];
	xblocks.clear();
}

void function::create_pieces()
{
	for (unsigned int i = 0; i < c_blocks.size(); ++i)
	{
		pieces.push_back(c_blocks[i]);
	}
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
{	//directly creates the blocks of code for the function based on a starting address
	address offset = 0;
	code_element *first;
	first = new code_element(s);
	c_blocks.push_back(first);

	for (unsigned int i = 0; i < c_blocks.size(); i++)
	{
		//std::cout << "Block " << i << " at addr 0x" << std::hex << c_blocks[i]->gets() << std::dec << std::endl;
		if (check_block(c_blocks[i]))
		{
		//	std::cout << "...doing block\n";
			while (c_blocks[i]->is_done() == 0)
			{
				try
				{
					instr *temp;
					int len;
					len = disas.get_instruction(temp, c_blocks[i]->gets()+offset);	//gather an instruction
					c_blocks[i]->add_line(temp);	//add it to the current block
					add_line(temp);	//add the line to the list of all lines for the function
					offset += len;	//adjust for the next instruction
					
					if (temp->trace_jump != 0)
					{
						std::cout << "Must trace a jump destination [" << *temp->trace_jump << "]\n\t"
								   << temp->comment << "\n";
						std::cout << "--------------------------------\n";
						c_blocks[i]->fprint(std::cout, 2);
						std::cout << "--------------------------------\n";
					}
					if ( (temp->destaddra != 0) && (temp->destaddrb != 0) )
					{	//it is the end of the block if the instruction has two destinations
						code_element *tmp = 0;
						c_blocks[i]->done();
						add_block(temp->destaddra, c_blocks[i]);
						add_block(temp->destaddrb, c_blocks[i]);
						offset = 0;
					}
					else if ( (temp->destaddra != 0) && (temp->destaddrb == 0) && (temp->destaddra != c_blocks[i]->gets()+offset) )
					{	//if the next instruction is not immediately after this one, it belongs to another block
						code_element *tmp = 0;
						c_blocks[i]->done();
						add_block(temp->destaddra, c_blocks[i]);
						offset = 0;
					}
					else if ( (temp->destaddrb != 0) && (temp->destaddra == 0) && (temp->destaddrb != c_blocks[i]->gets()+offset) )
					{	//if the next instruction is not immediately after this one, it belongs to another block
						code_element *tmp = 0;
						c_blocks[i]->done();
						add_block(temp->destaddrb, c_blocks[i]);
						offset = 0;
					}
					else if ( (temp->destaddra == 0) && (temp->destaddrb == 0) )
					{	//it is the end of the block if the instruction goes nowhere
						c_blocks[i]->done();
						offset = 0;
					}
				}
				catch (block_already_done &e)
				{	//don't work on the block if it is already done
					std::cout << "Block already done\n";
				}
			}
		}
		else
		{
			code_element *old = c_blocks[i];
			c_blocks.erase(c_blocks.begin()+i);
			code_element *replace = find_block(old->gets());
			replace_cblock_references(old, replace);
			delete old;
			i--;
		}
	}
}

void function::link_blocks()
{
}

void function::add_line(instr *addme)
{
	da_lines.push_back(addme);
}

//check to see if another block already duplicates this one
int function::check_block(code_element *ref)
{
	int add_it = 1;
	
	for (unsigned int i = 0; i < c_blocks.size(); ++i)
	{
		if (c_blocks[i]->gets() != ref->gets())
		{
			int res = c_blocks[i]->contains(ref->gets());
			if (res == -1)
			{	//split the block
				code_element *tmp;
				code_element *temp;
				temp = c_blocks[i]->split(ref->gets());
				temp->add_input(ref);
				tmp = find_block(temp->getline(-1)->destaddra);
				if (tmp != 0)
				{
					temp->set_a(tmp);
				}
				tmp = find_block(temp->getline(-1)->destaddrb);
				if (tmp != 0)
				{ 
					temp->set_b(tmp);
				}
				c_blocks.push_back(temp);
				add_it = 0;
			}
			else if (res == 1)
			{
				add_it = 0;
			}
		}
	}
	return add_it;
}

code_element *function::find_block(address start)
{
	for (unsigned int i = 0; i < c_blocks.size(); ++i)
	{
		if (c_blocks[i]->gets() == start)
			return c_blocks[i];
	}
	return 0;
}

void function::add_block(address addr, code_element *ref)
{	//consider adding a block starting at address addr
	//std::cout << "Adding new block at addr: 0x"
	//		  << std::hex << addr << std::dec << std::endl;
	int add_it = 1;
	for (unsigned int i = 0; i < c_blocks.size(); ++i)
	{
		int res = c_blocks[i]->contains(addr);
			
		if (res == 1)
		{
			code_element *tmp;
			c_blocks[i]->add_input(ref);
			tmp = find_block(ref->getline(-1)->destaddra);
			if (tmp != 0)
				ref->set_a(tmp);
			tmp = find_block(ref->getline(-1)->destaddrb);
			if (tmp != 0)
				ref->set_b(tmp);
			add_it = 0;
			break;
		}
		else if (res == -1)
		{	//split the block
			code_element *tmp;
			code_element *temp;
			temp = c_blocks[i]->split(addr);
			temp->add_input(ref);
			c_blocks.push_back(temp);
			tmp = find_block(temp->getline(-1)->destaddra);
			if (tmp != 0)
				temp->set_a(tmp);
			tmp = find_block(temp->getline(-1)->destaddrb);
			if (tmp != 0)
				temp->set_b(tmp);
			add_it = 0;
		}
	}
	if (add_it)
	{
		code_element *temp = new code_element(addr);
		temp->add_input(ref);
		if (ref->getline(-1)->destaddra == temp->gets())
		{
			ref->set_a(temp);
		}
		if (ref->getline(-1)->destaddrb == temp->gets())
		{
			ref->set_b(temp);
		}
		c_blocks.push_back(temp);
	}
}

void function::set_name(const char *to)
{
	name = to;
}

void function::set_type(type t)
{
	ret_type = t;
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

void function::replace_cblock_references(code_element *old, code_element *nw)
{
	unsigned int i;
	for (i = 0; i < c_blocks.size(); i++)
	{
		if (c_blocks[i] == old)
			c_blocks[i] = nw;
		c_blocks[i]->replace_references(old, nw);
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
		//find_stuff(c_blocks, actual_num_blocks, 1);
		//std::cout << "Reduced by " << reduced << " blocks\n";
	} while (reduced > 0);
	if (pieces.size() > 1)
	{
		//std::cout << "Failed to simplify enough (" << pieces.size() << "/" << c_blocks.size() << ")\n";
		for (int i = 0; i < pieces.size(); i++)
		{
			address temp1 = 0;
			if (pieces[i]->ga() != 0)
				temp1 = pieces[i]->ga()->gets();
			address temp2 = 0;
			if (pieces[i]->gb() != 0)
				temp2 = pieces[i]->gb()->gets();
			//std::cout << "Block " << i << ": 0x" << std::hex << pieces[i]->gets()
			//		   << " flows to either 0x" << temp1 << " or 0x" 
			//		   << temp2 << std::dec << std::endl;
		}
	}
	else
	{
		//std::cout << "Logic fully reduced\n";
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
	//output << "//There are " << pieces.size() << " blocks\n";
	output << ret_type.get_name() << " " << name << "(";

	if (arguments.size() > 0)
	{
		output << arguments[0];
	}
	for (i = 1; i < arguments.size(); i++)
	{
		output << ", ", arguments[i];
	}

	output << ")\n{\n";
	for (i = 0; i < pieces.size(); i++)
	{
		//output << "***************************** " << i << " ";
		//output << std::hex << pieces[i] 
		//	   << " 0x" << pieces[i]->gets() << std::dec << "\n";
		if ((pieces[i]->ga() != 0) && (pieces[i]->gb() != 0))
		{
			output << "#error \"Unfinished block\"\n";
		}
		pieces[i]->fprint(output, 1);
	}
	output << "}\n";
}

void function::output_graph_data(std::string fld_name)
{
	std::string outname = fld_name + "/" + name + ".gv";
	std::ofstream output;
	output.open(outname, std::ios::out);
	output << "digraph " << name << "{\n";
	output << std::hex;
	for (int i = 0; i < pieces.size(); i++)
	{
		pieces[i]->print_graph(output);
	}
	output << "}\n";
	output.close();
	
	simplify();
	
	outname = fld_name + "/" + name + "sim.gv";
	output.open(outname, std::ios::out);
	output << "digraph " << name << "{\n";
	output << std::hex;
	for (int i = 0; i < pieces.size(); i++)
	{
		pieces[i]->print_graph(output);
	}
	output << "}\n";
	output.close();
}

void function::output_code(std::string fld_name)
{
	simplify();
	std::string outname = fld_name + "/" + name + ".c";
	std::ofstream output;
	output.open(outname, std::ios::out);
	fprint(output);
	output.close();
}

int function::find_runs()
{
	unsigned int i;
	int found = 0;
	for (i = 0; i < pieces.size(); i++)
	{
		if ((pieces[i]->is_cbranch() != 1) && 
			(pieces[i]->ga() != 0) &&
			(pieces[i]->gains() == 1) )
		{
			std::vector<code_element*> lisp;
			code_run *temp;
			temp = new code_run;
			unsigned int j;
			lisp.push_back(pieces[i]);
			while ((lisp.back()->is_cbranch() != 1) && (lisp.back()->ga() != 0) && 
					(lisp.back()->gains() == 1))
			{
				lisp.push_back(lisp.back()->ga());
			}
			if (lisp.size() > 1)
			{
				replace_references(pieces[i], temp);
				for (j = 0; j < lisp.size(); j++)
				{
					temp->add_element(lisp[j]);
					if (j != 0)
						remove_piece(lisp[j]);
				}
				temp->done();
				found++;
			}
			xblocks.push_back(temp);
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
			replace_references(pieces[i], temp);
			xblocks.push_back(temp);
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
			replace_references(pieces[i]->ga(), temp);
			remove_piece(pieces[i]);
			xblocks.push_back(temp);
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
		replace_references(pieces[i], temp);
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
		return 1;
	}
	if ( (a->gins() == 1) &&
		 (b->gins() == 1) &&
	     (a->ga() == 0) && 
	     (a->is_cbranch() != 1) )
	{
		code_if_else *temp;
		temp = new code_if_else;
		temp->add_lcb(pieces[i]);
		temp->add_ecb(a);
		replace_references(pieces[i], temp);
		if (b->gins() >= 1)
		{
			temp->set_last(b);
			remove_piece(b);
		}
		xblocks.push_back(temp);
		remove_piece(a);
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
		if ( (list.back()->gains() > 1) &&
			(list.back()->gbins() == 1) )
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
		else if ( (list.back()->gbins() > 1) &&
			(list.back()->gains() == 1) )
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
						replace_references(pieces[i], temp);
						remove_piece(common);
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
						replace_references(pieces[i], temp);
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
						if (common->gains() > 1)
						{
							//common->ga()->dins(1);
						}
						else if (common->gains() == 1)
						{
							remove_piece(common->ga());
						}
						xblocks.push_back(temp);
						found++;
					}
					else if ( (common->is_cbranch() != 1) && (list.back()->is_cbranch() == 1) &&
							(list.back()->ga()->ga() == list.back()->gb()->ga()) &&
							(common->ga() != 0) &&
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
						replace_references(pieces[i], temp);
						remove_piece(common);
						remove_piece(list.back()->gb());
						if (common->gains() > 1)
						{
							//common->ga()->dins(1);
						}
						else if (common->gains() == 1)
						{
							remove_piece(common->ga());
						}
						xblocks.push_back(temp);
						found++;
					}
					else if ( (common->is_cbranch() != 1) && (list.back()->is_cbranch() == 1) &&
							(list.back()->gb()->ga() == list.back()->ga()->ga()) &&
							(common->ga() != 0) &&
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
						replace_references(pieces[i], temp);
						remove_piece(common);
						remove_piece(list.back()->ga());
						if (common->gains() > 1)
						{
							//common->ga()->dins(1);
						}
						else if (common->gains() == 1)
						{
							remove_piece(common->ga());
						}
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
		if ( (lcb.back()->ga()->is_cbranch() != 1) &&
			 (lcb.back()->gb()->is_cbranch() != 1) && 
			 (lcb.back()->ga()->ga() == 0) &&
			 (lcb.back()->gb()->ga() == 0) )
		{
			ecb.push_back(lcb.back()->ga());
			helse = lcb.back()->gb();
			
			code_if_else *temp;
			temp = new code_if_else;
			replace_references(pieces[i], temp);
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
			xblocks.push_back(temp);
			found++;
			done = 1;
		}
		if ( (lcb.back()->ga()->is_cbranch() == 1) &&
			 (lcb.back()->gb()->is_cbranch() != 1) )
		{	//a is conditional, b is not
			if ( (lcb.back()->ga()->gbins() == 1) &&
				 (lcb.back()->gbins() == 1) )
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
			if ( (lcb.back()->gb()->gbins() == 1) &&
				 (lcb.back()->gains() == 1) )
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
		{	//neither a nor b are conditional, both go somewhere though
			if ( (lcb.back()->gains() == 1) &&
				 (lcb.back()->gbins() == 1) &&
				 (lcb.back()->ga()->ga() != 0) &&
				 (lcb.back()->ga()->gains() >= (ecb.size()+2)) )
			{
				ecb.push_back(lcb.back()->ga());
				helse = lcb.back()->gb();
				code_if_else *temp;
				temp = new code_if_else;
				replace_references(pieces[i], temp);
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
				if (helse->gains() == (ecb.size()+1))
				{
					temp->set_last(helse->ga());
					remove_piece(helse->ga());
				}
				else
				{
					temp->set_next(helse->ga());
					//helse->ga()->dins(ecb.size());
				}
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
					replace_references(pieces[i], temp);
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
					replace_references(pieces[i], temp);
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
				if ( (pieces[i]->gains() == 1) &&
						(pieces[i]->gbins() == 1))
				{
					found += do_if_else(i);
				}
			}
			if (found == 0)
			{
				if ( (pieces[i]->gbins() > 1) ||
					(pieces[i]->gains() > 1) )
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
