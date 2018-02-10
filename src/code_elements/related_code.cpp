#include "related_code.h"

#include "ce_basic.h"
#include "code_do_while_loop.h"
#include "code_if_else.h"
#include "code_run.h"
#include "code_while_loop.h"
#include "var/combo.h"

std::vector<code_element_maker> *related_code::rc_makers = 0;

related_code::related_code()
{
}

void related_code::register_code_element_maker(code_element_maker a)
{
	if (related_code::rc_makers == 0)
	{
		related_code::rc_makers = new std::vector<code_element_maker>();
	}
	related_code::rc_makers->push_back(a);
}

void related_code::list_code_element_makers(std::ostream &dest)
{
	dest << "Registered code_element makers:" << std::endl;
	if (related_code::rc_makers != 0)
	{
		for (unsigned int i = 0; i < related_code::rc_makers->size(); i++)
		{
			dest << "Item " << i << " cannot be printed yet" << std::endl;
		}
	}
	else
	{
		dest << "None" << std::endl;
	}
}

void related_code::add_block(code_element *c)
{
	blocks.push_back(c);
}

void related_code::get_calls(std::vector<address> &c)
{
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		blocks[i]->get_calls(c);
	}
}

code_element *related_code::get_block(address a)
{
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		if (blocks[i]->gets() == a)
			return blocks[i];
	}
	return (code_element*)0;
}

void related_code::finalize_blocks()
{
	for (unsigned int i = 0; i < blocks.size(); i++)
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
	if (blocks.size() > 0)
	{
		if (simplified())
		{
			code_element *ref = blocks[0];
			while (ref != 0)
			{
				ref->fprint(dest, depth);
				ref = ref->a;
			}
		}
		else
		{
			for (unsigned int i = 0; i < blocks.size(); i++)
			{
				std::vector<address> p = blocks[i]->get_nexts();
				if (p.size() > 1)
				{
					dest << "#error Unfinished block" << std::endl;
				}
				blocks[i]->fprint(dest, depth);
			}
		}
	}
}

void related_code::print_graph(std::ostream &dest)
{
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		blocks[i]->print_graph(dest);
	}
}

void related_code::replace_element(code_element* old, code_element *n)
{
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		blocks[i]->replace_references(old, n);
	}
	for (unsigned int i = 0; i < blocks.size(); i++)
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
	for (unsigned int i = 0; i < gr.size(); i++)
	{
		if (gr[i]->gets() == a)
			return true;
	}
	return false;
}

bool reference_present(std::vector<code_element*> gr, address a)
{
	for (unsigned int i = 0; i < gr.size(); i++)
	{
		if ( ((gr[i]->a != 0) && (gr[i]->a->gets() == a)) ||
			 ((gr[i]->b != 0) && (gr[i]->b->gets() == a)) )
			return true;
	}
	return false;
}

bool non_self_reference(std::vector<code_element*> gr, address a)
{
	for (unsigned int i = 0; i < gr.size(); i++)
	{
		if ( ( ((gr[i]->a != 0) && (gr[i]->a->gets() == a)) ||
			   ((gr[i]->b != 0) && (gr[i]->b->gets() == a)) )
			&&
			(gr[i]->gets() != a)
			)
			return true;
	}
	return false;
}

unsigned int get_index(std::vector<code_element*> gr, code_element *b)
{
	unsigned int ret = gr.size();
	for (unsigned int i = 0; i < gr.size(); i++)
	{
		if (gr[i] == b)
		{
			ret = i;
		}
	}
	return ret;
}

bool no_dead_ends(std::vector<code_element*> gr)
{
	for (unsigned int i = 0; i < gr.size(); i++)
	{
		if ((gr[i]->a == 0) && (gr[i]->b == 0) )
			return false;
	}
	return true;
}


void related_code::replace_group(std::vector<code_element*>a, code_element *b)
{
	replace_element(a[0], b);
	a.erase(a.begin());
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		if (element_present(a, blocks[i]->gets()))
		{
			blocks.erase(blocks.begin() + i);
			i--;
		}		
	}
}

//processes a group of code_elements, returning the number of elements not in the group that point into the group 
std::vector<code_element *> related_code::external_inputs(std::vector<code_element *> gr)
{
	std::vector<code_element*>out_mods;
	//blocks = combination of gr and out_mods
	std::vector<code_element*> refed_mods;	//a subset of gr that is referenced from elements not in gr
	//add all modules not in the group
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		if (!element_present(gr, blocks[i]->gets()))
		{
			out_mods.push_back(blocks[i]);
		}
	}
	for (unsigned int i = 0; i < gr.size(); i++)
	{
		if (!reference_present(blocks, gr[i]->gets()))
		{
			refed_mods.push_back(gr[i]);
		}
	}
	for (unsigned int i = 0; i < out_mods.size(); i++)
	{
		if (out_mods[i]->a != 0)
		{
			address temp = out_mods[i]->a->gets();
			if (element_present(gr, temp) && !element_present(refed_mods, temp))
			{
				refed_mods.push_back(out_mods[i]->a);
			}
		}
		if (out_mods[i]->b != 0)
		{
			address temp = out_mods[i]->b->gets();
			if (element_present(gr, temp) && !element_present(refed_mods, temp))
			{
				refed_mods.push_back(out_mods[i]->b);
			}
		}
	}
	return refed_mods;
}

//processes a group of code_elements, returning the number of references that are not in that group
std::vector<code_element *> related_code::outside_references(std::vector<code_element *> gr)
{
	std::vector<code_element*>out_mods;
	//blocks = combination of gr and out_mods
	std::vector<code_element*> refed_mods;	//a subset of gr that is referenced from elements not in gr
	//add all modules not in the group
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		if (!element_present(gr, blocks[i]->gets()))
		{
			out_mods.push_back(blocks[i]);
		}
	}
	for (unsigned int i = 0; i < gr.size(); i++)
	{
		if (gr[i]->a != 0)
		{
			address temp = gr[i]->a->gets();
			if (element_present(out_mods, temp) && !element_present(refed_mods, temp))
			{
				refed_mods.push_back(gr[i]->a);
			}
		}
		if (gr[i]->b != 0)
		{
			address temp = gr[i]->b->gets();
			if (element_present(out_mods, temp) && !element_present(refed_mods, temp))
			{
				refed_mods.push_back(gr[i]->b);
			}
		}
	}
	return refed_mods;
}

bool related_code::next_combo(std::vector<unsigned int> &cmb)
{
	unsigned int i = cmb.size();
	bool done = false;
	
	while (!done)
	{
	
		if ((cmb[i-1]+1) < (blocks.size() - cmb.size() + i))
		{
			cmb[i-1]++;
			if (i < cmb.size())
			{
				for (unsigned int j = i; j < cmb.size(); j++)
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
	for (unsigned int i = 0; i < gr.size(); i++)
	{
		gr[i] = blocks[cmb[i]];
	}
}

int related_code::process_blocks(int n)
{
	int result = 0;
	int index = 0;
	while ((result == 0) && (index < blocks.size()))
	{
		combo cur_combo(blocks[index++], n);
		while (cur_combo.valid())
		{
			std::vector<code_element *> group = cur_combo.get_combination();
			
			cur_combo.next_combo();
			std::vector<code_element *> ex_in = external_inputs(group); 
			unsigned int ext_in = ex_in.size();
			std::vector<code_element *> ex_out = outside_references(group); 
			if (ex_out.size() == 0)
			{
				ex_out.push_back(0);
			}
			unsigned int ext_out = ex_out.size();
			
			if ( (ext_in == 1) && (ext_out == 1))
			{
				if (group[0] != ex_in[0])
				{
					for (unsigned int i = 1; i < group.size(); i++)
					{
						if (group[i] == ex_in[0])
						{
							code_element *temp;
							temp = group[0];
							group[0] = group[i];
							group[i] = temp;
						}
					}	
				}
				
				std::cout << std::hex << "Valid group: ";
				for (unsigned int i = 0; i < group.size(); i++)
				{
					std::cout << "0x" << group[i]->gets() << ", ";
				}
				if (ex_out[0] != 0)
					std::cout << "(0x" << ex_out[0]->gets() << ")";
				std::cout << std::dec << std::endl;
				
				if (related_code::rc_makers != 0)
				{
					for (unsigned int i = 0; i < related_code::rc_makers->size(); i++)
					{
						code_element *temp =
							((*related_code::rc_makers)[i])(group, ex_out[0]);
						if (temp != 0)
						{
							result++;
							replace_group(group, temp);
						}
					}
				}
			}
		}
	}
	return result;
}

bool related_code::simplified()
{
	bool ret = false;
	//all elements must be non-branching
	bool all_nonbranch = true;
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		if (blocks[i]->is_branch())
		{
			all_nonbranch = false;
		}
	}
	if (all_nonbranch)
	{
		ret = true;
	}
	int number_deads = 0;
	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		if ( (blocks[i]->a == 0) && (blocks[i]->b == 0))
		{
			number_deads++;
		}
	}
	if (number_deads > 1)
	{
		ret = false;
	}
	
	return ret;
}

void related_code::simplify()
{
	int blocks_done;
	int section_done;
	unsigned int num_blocks = 2;
	do
	{
		blocks_done = 0;
		section_done = 0;

		while (num_blocks <= blocks.size())
		{
			section_done = process_blocks(num_blocks);
			blocks_done += section_done;
			if (section_done == 0)
				num_blocks++;
		}
	} while ((blocks_done > 0) && (num_blocks < blocks.size()));
}

