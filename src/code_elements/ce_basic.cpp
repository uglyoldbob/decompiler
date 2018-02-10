#include "ce_basic.h"

#include "exceptions.h"
#include "helpers.h"

ce_basic::ce_basic(address addr) : code_element(addr)
{
	is_branch = false;
}

ce_basic *ce_basic::first_half(address addr)
{
	ce_basic *ret = new ce_basic(s);
	unsigned int i = 0;
	while (lines[i].addr != addr)
	{
		ret->lines.push_back(lines[i]);
		i++;
	}
	return ret;
}

ce_basic *ce_basic::second_half(address addr)
{
	ce_basic *ret = new ce_basic(addr);
	unsigned int i = 0;
	while (lines[i].addr != addr)
	{
		i++;
	}
	while (i < lines.size())
	{
		ret->lines.push_back(lines[i]);
		i++;
	}
	return ret;
}

int ce_basic::contains(address addr)
{
	for (unsigned int i = 0; i < lines.size(); ++i)
	{
		if (lines[i].addr == addr)
			return -1;	//the block starting at this address exists inside another block
	}
	return 0;
}

bool ce_basic::should_be_added(address addr)
{	//returns true if the address should be added to this block
	if (lines.size() == 0)
	{
		if (s == addr)
			return true;
		else
			return false;
	}
	instr end = lines.back();
	if ((end.is_branch) || (this->is_branch))
	{
		return false;
	}
	else
	{
		if (end.destaddra == addr)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

//Returns the addresses this element might flow to
std::vector<address> ce_basic::get_nexts()
{
	std::vector<address> ret;

	if (lines.size() != 0)
	{
		instr end = lines.back();
		if (!end.is_ret)
		{
			ret.push_back(end.destaddra);
		}
		if (end.is_cbranch)
		{
			ret.push_back(end.destaddrb);
		}	
	}
	return ret;
}

void ce_basic::add_line(instr *addme)
{
	lines.push_back(*addme);
}

void ce_basic::get_calls(std::vector<address> &c)
{
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		if (lines[i].call != 0)
			c.push_back(lines[i].call);
	}
}

void ce_basic::fprint(std::ostream &dest, int depth)
{
	for (unsigned int k = 0; k < lines.size(); k++)
	{
		std::stringstream temp;
		temp << tabs(depth);
		std::string tmpstr(temp.str());
		lines[k].preprint = tmpstr;
		dest << lines[k] << "\n";
		lines[k].preprint = "";
	}
}
