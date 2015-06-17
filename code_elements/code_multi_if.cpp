#include "code_multi_if.h"
#include "helpers.h"

code_multi_if::code_multi_if()
{
	form = NONE;
	helse = 0;
	thefinal = 0;
}

code_multi_if::~code_multi_if()
{
	form = NONE;
}

void code_multi_if::add(code_element *ad)
{
	ifs.push_back(ad);
	if (ifs.size() == 1)
	{
		copy_inputs(ad);
		s = ad->gets();
	}
}

void code_multi_if::common(code_element *c)
{
	common_block = c;
	if (ifs.back()->gb() != c)
	{
		a = ifs.back()->gb();
	}
	else if (ifs.back()->ga() != c)
	{
		a = ifs.back()->ga();
	}

/*	if (c->ga() != a)
	{
		b = c->ga();
	}
	else
	{
		c->ga()->dins(1);
	}*/
}

void code_multi_if::fprint(std::ostream &dest, int depth)
{
	unsigned int i;
	dest << tabs(depth) << "/------";
	if (depth == 0)
	{
		dest << std::hex << s << std::dec << " (" << inputs.size() << " input)";
	}
	else
	{
		dest << std::hex << s << std::dec << " (" << inputs.size() << " input) ";
	}
	for (int i = 0; i < inputs.size(); i++)
	{
		dest << std::hex << inputs[i]->gets() << std::dec << " ";  
	}
	dest << "\n";
	ifs[0]->fprint(dest, depth);
	dest << tabs(depth) << "if ( (?)";
	if (form == AND_NO_ELSE)
	{
		for (i = 1; i < ifs.size(); i++)
		{
			dest << " && (?)";
		}
	}
	else
	{
		for (i = 1; i < ifs.size(); i++)
		{
			dest << " || (?)";
		}
	}
	dest << " )\n";
	dest << tabs(depth) << "{\n";
	common_block->fprint(dest, depth+1);
	dest << tabs(depth) << "}\n";
	if (helse != 0)
	{
		dest << tabs(depth) << "else\n";
		dest << tabs(depth) << "{\n";
		helse->fprint(dest, depth+1);
		dest << tabs(depth) << "}\n";
	}
	
	if (thefinal != 0)
	{
		thefinal->fprint(dest, depth);
	}
	dest << tabs(depth) << "\\------ ";
	if (depth == 0)
	{
		if (a != 0)
			dest << std::hex << a->gets() << std::dec << " ";
		else
			dest << "NULL ";
		if (b != 0)
			dest << std::hex << b->gets() << std::dec << " ";
		else
			dest << "NULL ";
	}
	else
	{
		if (a != 0)
			dest << std::hex << a->gets() << std::dec << " ";
		else
			dest << "NULL ";
		if (b != 0)
			dest << std::hex << b->gets() << std::dec << " ";
		else
			dest << "NULL ";
	}
	dest << "\n";
}

void code_multi_if::print_graph(std::ostream &dest)
{
	dest << "#multi if\n";
	for (int i = 0; i < ifs.size(); i++)
	{
		ifs[i]->print_graph(dest);
	}
	if (common_block != 0)
		common_block->print_graph(dest);
	if (helse != 0)
		helse->print_graph(dest);
	if (thefinal != 0)
		thefinal->print_graph(dest);
	dest << "#end multi if\n";
}

void code_multi_if::set_else(code_element *e)
{
	helse = e;
}

void code_multi_if::set_final(code_element *f)
{
	if (f->gins() == 1)
	{
		thefinal = f;
		a = f->ga();
		b = f->gb();
	}
	else if (f->gins() > 1)
	{
		a = f;
		b = 0;
	}
}

void code_multi_if::finish_and_no_else()
{
	form = AND_NO_ELSE;
}

void code_multi_if::finish_or_no_else()
{
	form = OR_NO_ELSE;
}

void code_multi_if::finish_with_else()
{
	form = WITH_ELSE;
}
