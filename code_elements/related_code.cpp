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
		int len;
		int belongs_to = -1;
		bool dont_add = false;
		len = disas.get_instruction(temp, cur_addr);
		for (int i = 0; i < blocks.size(); i++)
		{
			if (blocks[i]->should_be_added(cur_addr))
			{
				belongs_to = i;
			}
			if (blocks[i]->contains(cur_addr))
			{
				dont_add = true;
			}	
		}
		if ((belongs_to == -1) && !dont_add)
		{	//create a block because it does not exist yet
			code_element *temp = new code_element(cur_addr);
			blocks.push_back(temp);
			belongs_to = blocks.size() - 1;
		}
		if (belongs_to != -1)
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

