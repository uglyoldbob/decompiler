#include "code_element.h"

#include "exceptions.h"
#include "helpers.h"

code_element::code_element(address addr)
	: s(addr)
{
	a = 0;
	b = 0;
	depth = 0;
	is_branch = false;
}

code_element::code_element(code_element* in, address start)
	: s(start)
{
	depth = 0;
	unsigned int i;
	for (i = 0; i < in->lines.size(); ++i)
	{
		if (in->lines[i].addr == start)
			break;
	}
	while (i < in->lines.size())
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
	in->is_branch = true;
}

code_element::code_element()
	: s(0)
{
	a = 0;
	b = 0;
	depth = 0;
}

code_element* code_element::split(address addr)
{
	code_element *ret;
	ret = new code_element(this, addr);
	return ret;
}

code_element::~code_element()
{
	for (int i = 0; i < lines.size();i++)
	{
		for (int j = 0; j < lines[i].statements.size(); j++)
		{
			delete lines[i].statements[j];
		}
		lines[i].statements.clear();
		if (lines[i].trace_call != 0)
			delete lines[i].trace_call;
		if (lines[i].trace_jump != 0)
			delete lines[i].trace_jump;
	}
	lines.clear();
	a = 0;
	b = 0;
}

int code_element::contains(address addr)
{
	for (unsigned int i = 0; i < lines.size(); ++i)
	{
		if (lines[i].addr == addr)
			return -1;	//the block starting at this address exists inside another block
	}
	return 0;
}

bool code_element::should_be_added(address addr)
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

bool code_element::no_end()
{	//returns true if this block does not have a proper end
	if (lines.size() == 0)
	{
		return true;
	}
	instr end = lines.back();
	return (!end.is_branch && !end.is_ret);
}

//Returns the addresses this element might flow to
std::vector<address> code_element::get_nexts()
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

void code_element::add_line(instr *addme)
{
	lines.push_back(*addme);
}

void code_element::replace_references(code_element *old, code_element *nw)
{
	if (a == old)
		a = nw;
	if (b == old)
		b = nw;
}

void code_element::print_graph(std::ostream &dest)
{
	if (lines.size() > 0)
	{
		instr end = lines.back();
		if (!end.is_ret)
			dest << "\tX" << s// << "_" << pieces[i] 
				 << " -> X" << end.destaddra // << "_" << pieces[i]->ga()  
				 << " [ label=a ];\n";
		if (end.is_cbranch)
			dest << "\tX" << s// << "_" << pieces[i] 
				 << " -> X" << end.destaddrb// << "_" << pieces[i]->gb() 
				 << " [ label=b ];\n";
		dest << "\tX" << s << ";\n";
	}
}

void code_element::fprint(std::ostream &dest, int depth)
{
	for (unsigned int k = 0; k < lines.size(); k++)
	{
		std::stringstream temp;
		std::string tmpstr(temp.str());
		temp << tabs(depth);
		lines[k].preprint = tmpstr;
		dest << lines[k] << "\n";
		lines[k].preprint = "";
	}
}

address code_element::gets()
{
	return s;
}

