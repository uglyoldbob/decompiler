#include "ce_block.h"
#include "exceptions.h"
#include "helpers.h"

ce_block::ce_block(address start)
	: code_element(start)
{
	finished = 0;
}

ce_block::ce_block(ce_block* in, address start)
	: code_element(start)
{
	finished = in->finished;
	unsigned int i;
	for (i = 0; i < in->lines.size(); ++i)
	{
		if (in->lines[i]->addr == start)
			break;
	}
	while (i < lines.size())
	{	//copy all lines to this
		//and delete them from in
		lines.push_back(in->lines[i]);
		in->lines.erase(in->lines.begin() + i);
	}
	//save the original destinations
	a = in->a;
	b = in->b;
	//now overwrite them
	in->a = this;
	in->b = 0;
	inputs.push_back(in);
}

ce_block::~ce_block()
{
	//lines are managed by the function class
}

ce_block* ce_block::split(address addr)
{
	ce_block *ret;
	ret = new ce_block(this, addr);

	return ret;
}

void ce_block::fprint(std::ostream &dest, int depth)
{
	dest << tabs(depth) << "/------";
	if (depth == 0)
		dest << std::hex << s << std::dec << " (" << lines[0]->ins << "input)";
	dest << "\n";	
	for (unsigned int k = 0; k < lines.size(); k++)
	{
		std::stringstream temp;
		temp << tabs(depth) << "|";
		lines[k]->preprint = temp.str();
		dest << *lines[k] << "\n";
		lines[k]->preprint = "";
	}
	dest << tabs(depth) << "\\------ ";
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

int ce_block::contains(address addr)
{
	if (lines.size() == 0)
	{
		if (s == addr)	//the block for this address has not had any lines added to it yet
			return 1;	//but it does exist
	}
	else
	{
		if (lines[0]->addr == addr)
			return 1;	//the block for this address already exists
		for (unsigned int i = 1; i < lines.size(); ++i)
		{
			if (lines[i]->addr == addr)
				return -1;	//the block starting at this address exists inside another block
		}
	}
	return 0;
}

int ce_block::work(address addr)
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
	else
	{
		for (unsigned int i = 0; i < lines.size(); i++)
		{
			if (lines[i]->addr == addr)
				return 1;	//address was found in this block
		}
	}
	return 0;	//address not found in this block
}

void ce_block::add_line(instr *addme)
{
	lines.push_back(addme);
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
