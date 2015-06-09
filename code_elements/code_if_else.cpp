#include "code_if_else.h"
#include "helpers.h"

code_if_else::code_if_else()
{
	helse = NULL;
	next = NULL;
}

code_if_else::~code_if_else()
{
}

void code_if_else::add_lcb(code_element *add)
{
	lcb.push_back(add);
	if (lcb.size() == 1)
	{
		copy_inputs(add);
		s = add->gets();
	}
}

void code_if_else::add_ecb(code_element *add)
{
	ecb.push_back(add);
}

void code_if_else::set_else(code_element *h)
{
	helse = h;
}

void code_if_else::set_last(code_element *l)
{
	next = l;
	a = l->ga();
	b = l->gb();
}

void code_if_else::set_next(code_element *n)
{
	a = n;
	for (unsigned int i = 0; i < lcb.size(); ++i)
		a->remove_input(lcb[i]);
	b = 0;
}

void code_if_else::fprint(std::ostream &dest, int depth)
{
	unsigned int i;
	lcb[0]->fprint(dest, depth);
	dest << tabs(depth) << "if (?)\n";
	dest << tabs(depth) << "{\n";
	ecb[0]->fprint(dest, depth+1);
	dest << tabs(depth) << "}\n";
	for (i = 1; i < lcb.size(); i++)
	{
		dest << tabs(depth) << "else if (?)\n";
		dest << tabs(depth) << "{\n";
		ecb[i]->fprint(dest, depth+1);
		dest << tabs(depth) << "}\n";
	}
	if (helse != 0)
	{
		dest << tabs(depth) << "else\n";
		dest << tabs(depth) << "{\n";
		helse->fprint(dest, depth+1);
		dest << tabs(depth) << "}\n";
	}
	if (next != 0)
	{
		next->fprint(dest, depth);
	}
}
