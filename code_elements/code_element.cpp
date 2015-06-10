#include "code_element.h"

#include "exceptions.h"
#include "helpers.h"

code_element::code_element(address addr)
	: s(addr)
{
	std::cout << "Init block at 0x" << std::hex << addr << std::dec << std::endl;
	a = 0;
	b = 0;
	depth = 0;
	finished = 0;
}

code_element::code_element(code_element* in, address start)
{
	std::cout << std::hex;
	std::cout << "Split block Init block 0x" 
			  << in->s << " at 0x" << start << std::endl;
	std::cout << std::dec;
	depth = 0;
	finished = in->finished;
	unsigned int i;
	for (i = 0; i < in->lines.size(); ++i)
	{
		if (in->lines[i]->addr == start)
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
	s = start;

	//now overwrite them
	in->a = this;
	in->b = 0;
	inputs.push_back(in);
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
	std::cout << std::hex;
	std::cout << "Split block Init block 0x" 
			  << s << " at 0x" << addr << std::endl;
	std::cout << std::dec;
	fprint(std::cout, 0);
	
	code_element *ret;
	ret = new code_element(this, addr);

	fprint(std::cout, 1);
	ret->fprint(std::cout, 1);
	
	return ret;
}

code_element::~code_element()
{
}

int code_element::is_cbranch()
{	//does this element have a conditional branch at the end
	if ((a != 0) && (b != 0))
		return 1;
	else
		return 0;
}

int code_element::is_done()
{
	return finished;
}

int code_element::contains(address addr)
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

instr *code_element::getline(int num)
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

void code_element::done()
{
	fprint(std::cout, 0);
	finished = 1;
}

void code_element::set_a(code_element *nwa)
{
	a = nwa;
}

void code_element::set_b(code_element *nwb)
{
	b = nwb;
}

int code_element::gins()	//get ins
{
	return inputs.size();
}

code_element *code_element::ga()
{
	if (a == 0)
		return b;
	else
		return a;
}

code_element *code_element::gb()
{
	if (a == 0)
		return a;
	else
		return b;
}

void code_element::add_line(instr *addme)
{
	lines.push_back(addme);
}

void code_element::replace_references(code_element *old, code_element *nw)
{
	if (a == old)
		a = nw;
	if (b == old)
		b = nw;
}

void code_element::copy_inputs(code_element *src)
{
	inputs = src->inputs;
}

void code_element::fprint(std::ostream &dest, int depth)
{
	dest << tabs(depth) << "/------";
	if (depth == 0)
	{
		dest << std::hex << s << std::dec << " (" << lines[0]->ins << "input)";
	}
	else
	{
		dest << std::hex << s << std::dec << " (" << lines[0]->ins << "input)";
	}
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
	else
	{
		if (a != 0)
			dest << std::hex << a->gets() << std::dec << " ";
		if (b != 0)
			dest << std::hex << b->gets() << std::dec << " ";
	}
	dest << "\n";
}

address code_element::gets()
{
	return s;
}

void code_element::add_input(code_element *ref)
{
	char add = 1;
	for (unsigned int i = 0; i < inputs.size(); ++i)
	{
		if (inputs[i] == ref)
		{
			add = 0;
			break;
		}
	}
	if (add)
		inputs.push_back(ref);
}

void code_element::remove_input(code_element *me)	//decrease ins
{
	for (unsigned int i = 0; i < inputs.size(); ++i)
	{
		if (inputs[i] == me)
		{
			inputs.erase(inputs.begin() + i);
			break;
		}
	}
	//cout << "Reduce ins of " << std::hex << s << std::dec << " (" << ins << ") by " << by << "\n";
	//ins -= by;
}
