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

bool code_if_else::check(code_element *e)
{
	return false;
}

void code_if_else::add_lcb(code_element *add)
{
	lcb.push_back(add);
	if (lcb.size() == 1)
	{
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
	a = l->a;
	b = l->b;
}

void code_if_else::set_next(code_element *n)
{
	a = n;
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

#ifdef PROVE_SIMPLIFY
void code_if_else::print_graph(std::ostream &dest)
{
	dest << "#if else\n";
	for (int i = 0; i < lcb.size(); i++)
		lcb[i]->print_graph(dest);
	for (int i = 0; i < ecb.size(); i++)
		ecb[i]->print_graph(dest);
	if (helse != 0)
		helse->print_graph(dest);
	if (next != 0)
		next->print_graph(dest);
	dest << "#end if else\n";
}
#endif
