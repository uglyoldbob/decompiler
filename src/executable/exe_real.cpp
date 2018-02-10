#include "exe_real.h"

#include "code_elements/ce_basic.h"

exe_real::exe_real(int reverse) : exe_loader(reverse)
{
}

exe_real::~exe_real()
{
}

std::vector<code_element *> exe_real::gather_instructions(address start_address)
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
		disasm->get_instruction(temp, cur_addr);

		for (unsigned int i = 0; i < gath.size(); i++)
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
		for (unsigned int i = 0; i < gath.size(); i++)
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
			for (unsigned int i = 0; i < p.size(); i++)
			{
				if (p[i] != 0)
					chunks.push_back(p[i]);
			}
		}
		chunks.erase(chunks.begin());
	}
	std::vector<code_element *> blocks;
	for (unsigned int i = 0; i < gath.size(); i++)
	{
		blocks.push_back(gath[i]);
	}
	return blocks;
}

