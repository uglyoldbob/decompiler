#include "related_code.h"

related_code::related_code()
{
}

void related_code::gather_instructions(disassembler &disas)
{
	std::vector<address> chunks;
	chunks.push_back(start_address);

	while (chunks.size() > 0)
	{
		address cur_addr = chunks.front();
		instr *temp;
		int belongs_to = -1;
		bool dont_add = false;
		disas.get_instruction(temp, cur_addr);

		for (int i = 0; i < blocks.size(); i++)
		{
			//find if the address is already part of an existing block
				//find if the address is already the start of a block
				//find if the address is already in a block but not the start
			//find if the address should be added to an existing block
				//add the address to the existing block
			//else
				//create a new block and add the address to the existing block
			if (blocks[i]->contains(cur_addr) != 0)
			{	//this address already exists in this block
				if (blocks[i]->gets() != cur_addr)
				{	//only do something if it is necessary to split the block
					code_element *sp = blocks[i]->split(cur_addr);
					blocks.push_back(sp);
					dont_add = true;
				}
				else
				{
					dont_add = true;
				}
			}
		}
		for (int i = 0; i < blocks.size(); i++)
		{
			if (blocks[i]->should_be_added(cur_addr))
			{	//add the address to the existing block
				belongs_to = i;
			}
		}
		if (blocks.size() == 0)
		{
			blocks.push_back(new code_element(cur_addr));
			belongs_to = 0;
		}
		if ((belongs_to == -1) && !dont_add)
		{
			code_element *temp = new code_element(cur_addr);
			blocks.push_back(temp);
			belongs_to = blocks.size() - 1;
		}
		if ((belongs_to != -1) && !dont_add)
		{	//add the line to the given block
			blocks[ belongs_to]->add_line(temp);
			std::vector<address> p = blocks[belongs_to]->get_nexts();
			for (int i = 0; i < p.size(); i++)
			{
				chunks.push_back(p[i]);
			}
		}
		chunks.erase(chunks.begin());
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

