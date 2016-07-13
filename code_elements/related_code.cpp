#include "related_code.h"

#include "ce_basic.h"
#include "code_do_while_loop.h"

related_code::related_code()
{
}

void related_code::gather_instructions(disassembler &disas)
{
	std::vector<ce_basic *> gath;
	std::vector<address> chunks;
	chunks.push_back(start_address);

	while (chunks.size() > 0)
	{
		address cur_addr = chunks.front();
		instr *temp;
		int belongs_to = -1;
		bool dont_add = false;
		disas.get_instruction(temp, cur_addr);

		for (int i = 0; i < gath.size(); i++)
		{
			//find if the address is already part of an existing block
				//find if the address is already the start of a block
				//find if the address is already in a block but not the start
			//find if the address should be added to an existing block
				//add the address to the existing block
			//else
				//create a new block and add the address to the existing block
			if (gath[i]->contains(cur_addr) != 0)
			{	//this address already exists in this block
				if (gath[i]->gets() != cur_addr)
				{	//only do something if it is necessary to split the block
					ce_basic *sp = gath[i]->second_half(cur_addr);
					gath.push_back(sp);
					gath[i] = gath[i]->first_half(cur_addr);
					dont_add = true;
				}
				else
				{
					dont_add = true;
				}
			}
		}
		for (int i = 0; i < gath.size(); i++)
		{
			if (gath[i]->should_be_added(cur_addr))
			{	//add the address to the existing block
				belongs_to = i;
			}
		}
		if (gath.size() == 0)
		{
			gath.push_back(new ce_basic(cur_addr));
			belongs_to = 0;
		}
		if ((belongs_to == -1) && !dont_add)
		{
			ce_basic *temp = new ce_basic(cur_addr);
			gath.push_back(temp);
			belongs_to = gath.size() - 1;
		}
		if ((belongs_to != -1) && !dont_add)
		{	//add the line to the given block
			gath[ belongs_to]->add_line(temp);
			std::vector<address> p = gath[belongs_to]->get_nexts();
			for (int i = 0; i < p.size(); i++)
			{
				chunks.push_back(p[i]);
			}
		}
		chunks.erase(chunks.begin());
	}
	for (int i = 0; i < gath.size(); i++)
	{
		blocks.push_back(gath[i]);
	}
	finalize_blocks();
}

code_element *related_code::get_block(address a)
{
	for (int i = 0; i < blocks.size(); i++)
	{
		if (blocks[i]->gets() == a)
			return blocks[i];
	}
	return (code_element*)0;
}

void related_code::finalize_blocks()
{
	for (int i = 0; i < blocks.size(); i++)
	{
		std::vector<address> p = blocks[i]->get_nexts();
		if (p.size() > 0)
		{
			blocks[i]->a = get_block(p[0]);
		}
		if (p.size() > 1)
		{
			blocks[i]->b = get_block(p[1]);
		}
	}
}

void related_code::fprint(std::ostream &dest, int depth)
{
	for (int i = 0; i < blocks.size(); i++)
	{
		std::vector<address> p = blocks[i]->get_nexts();
		if (p.size() > 1)
		{
			dest << "#error Unfinished block" << std::endl;
		}
		blocks[i]->fprint(dest, depth);
	}
}

void related_code::print_graph(std::ostream &dest)
{
	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i]->print_graph(dest);
	}
}

void related_code::replace_element(code_element* old, code_element *n)
{
	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i]->replace_references(old, n);
	}
	for (int i = 0; i < blocks.size(); i++)
	{
		if (blocks[i] == old)
		{
			blocks[i] = n;
		}
	}
}

//returns true if an element startting with this address is present in the given group
bool element_present(std::vector<code_element*> gr, address a)
{
	for (int i = 0; i < gr.size(); i++)
	{
		if (gr[i]->gets() == a)
			return true;
	}
	return false;
}

//processes a group of code_elements, returning the number of elements not in the group that point into the group 
int related_code::external_inputs(std::vector<code_element *> gr)
{
	std::vector<code_element*>out_mods;
	//add all modules not in the group
	for (int i = 0; i < blocks.size(); i++)
	{
		if (!element_present(gr, blocks[i]->gets()))
		{
			out_mods.push_back(blocks[i]);
		}
	}
}

std::vector<unsigned int> make_combination(int num)
{
	std::vector<unsigned int> ret;
	for (int i = 0; i < num; i++)
	{
		ret.push_back(i);
	}
	return ret;
}

std::vector<code_element *> make_group(int num)
{
	std::vector<code_element*> ret;
	for (int i = 0; i < num; i++)
	{
		ret.push_back(0);
	}
	return ret;
}

bool related_code::next_combo(std::vector<unsigned int> &cmb)
{
	int i = cmb.size();
	bool done = false;
	
	while (!done)
	{
	
		if ((cmb[i-1]+1) < (blocks.size() - cmb.size() + i))
		{
			cmb[i-1]++;
			if (i < cmb.size())
			{
				for (int j = i; j < cmb.size(); j++)
				{
					cmb[j] = cmb[j-1]+1;
				}
			}
			done = true;
		}
		else
		{
			if (i > 1)
			{
				i--;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

void related_code::apply_combination(std::vector<unsigned int> cmb, std::vector<code_element *> &gr)
{	//retrieves combination "number" of all possibilities that have quant items of the blocks group
	for (int i = 0; i < gr.size(); i++)
	{
		gr[i] = blocks[cmb[i]];
	}
}

int related_code::process_blocks(int n)
{
	std::cout << "Start processing " << n << ", " << blocks.size() << std::endl;
	std::vector<unsigned int> cur_combo = make_combination(n);
	std::vector<code_element *> group = make_group(n);
	do
	{
		apply_combination(cur_combo, group);
		std::cout << "\tCur combo: ";
		for (int i = 0; i < cur_combo.size(); i++)
		{
			std::cout << cur_combo[i] << ", ";
		}
		std::cout << std::endl;
	} while (next_combo(cur_combo));
	std::cout << "End processing" << std::endl;
	return 0;
}

//processes a group of code_elements, returning the number of references that are not in that group
int related_code::outside_references(std::vector<code_element *> gr)
{
	int outside_refs = 0;
	for (int i = 0; i < gr.size(); i++)
	{
		bool internal_ref = false;
		for (int j = 0; j < gr.size(); j++)
		{
			if ( (gr[i]->a == gr[j]) || (gr[i]->b == gr[j]) )
				internal_ref = true;
		}
		if (!internal_ref)
		{
			outside_refs++;
		}
	}
	return outside_refs;
}

void related_code::simplify()
{
	std::cout << "Simplifying" << std::endl;
	int blocks_done;
	do
	{
		blocks_done = 0;
		for (int i = 0; i < blocks.size(); i++)
		{
			if (code_do_while_loop::check(blocks[i]))
			{
				std::cout << "Found a do while loop 0x" << std::hex << blocks[i]->gets() << std::dec << std::endl;
				code_do_while_loop *new_element = new code_do_while_loop(blocks[i]);
				replace_element(blocks[i], new_element);
				blocks_done++;
			}
		}
		blocks_done += process_blocks(3);
	} while (blocks_done > 0);
}

