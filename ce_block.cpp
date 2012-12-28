#include "ce_block.h"
#include "code_element.h"
#include "exceptions.h"

ce_block::ce_block()
{
	finished = 0;
}

ce_block::ce_block(const ce_block &src, address start, address end)
{	//used to fragment a block
	//find the starting line
	unsigned int start_b = 0;
	for (unsigned int i = 0; i < src.lines.size(); i++)
	{
		if (src.lines[i]->addr == start)
		{
			start_b = i;
			break;
		}
	}
	for (unsigned int i = start_b; i < src.lines.size(); i++)
	{
		lines.push_back(src.lines[i]);
		if (src.lines[i]->addr == end)
			break;
	}
}

ce_block::~ce_block()
{
	//lines are managed by the function class
}

void ce_block::fprint(std::ostream &dest, int depth)
{
	begin_line(dest, depth);
	dest << "/------";
	if (depth == 0)
		dest << std::hex << s << std::dec << " (" << lines[0]->ins << "input)";
	dest << "\n";	
	for (unsigned int k = 0; k < lines.size(); k++)
	{
		begin_line(dest, depth);
		dest << "|"
			 << std::hex << lines[k]->addr << std::dec << " "
			 << lines[k]->opcode << " " << lines[k]->options << " " << lines[k]->comment << "\n";
	}
	begin_line(dest, depth);
	dest << "\\------ ";
	if (depth == 0)
	{
		if (a != 0)
			dest << std::hex << a->gets() << std::dec << " ";
		if (b != 0)
			dest << std::hex << b->gets() << std::dec << " ";
	}
	dest << "\n";
}

instr *ce_block::getline(int num)
{	//-1 means get last line
	if (lines.size() == 0)
	{
		return NULL;
	}
	else if (num < -1)
	{
		return NULL;
	}
	else if (num == -1)
	{
		return lines[lines.size()-1];
	}
	else if ((unsigned int)num < lines.size())
	{
		return lines[num];
	}
	return 0;
}

unsigned int ce_block::getnline()
{
	return lines.size();
}

void ce_block::work(address addr)
{
	if (finished)
	{
		//check to see if the entire block is done
		if (lines[0]->addr == addr)
			throw block_already_done(addr);
		for (unsigned int i = 1; i < lines.size(); i++)
		{
			if (lines[i]->addr == addr)
				throw block_should_be_split(lines[i-1]->addr);
		}
	}
}

int ce_block::is_done()
{
	return finished;
}

void ce_block::done()
{	//the block is done, but may contain more than one block
	//one of the lines (besides the first one) could be the destination of a conditional branch
	finished = 1;
}
