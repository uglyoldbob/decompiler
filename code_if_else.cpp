#include "code_if_else.h"

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
		ins = add->gins();
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
	unsigned int i;
	a->dins(lcb.size());
	b = 0;
}

void code_if_else::fprint(FILE *dest, int depth)
{
	unsigned int i;
	lcb[0]->fprint(dest, depth);
	begin_line(dest, depth);
	fprintf(dest, "if (?)\n");
	begin_line(dest, depth);
	fprintf(dest, "{\n");
	ecb[0]->fprint(dest, depth+1);
	begin_line(dest, depth);
	fprintf(dest, "}\n");
	for (i = 1; i < lcb.size(); i++)
	{
		begin_line(dest, depth);
		fprintf(dest, "else if (?)\n");
		begin_line(dest, depth);
		fprintf(dest, "{\n");
		ecb[i]->fprint(dest, depth+1);
		begin_line(dest, depth);
		fprintf(dest, "}\n");
	}
	if (helse != 0)
	{
		begin_line(dest, depth);
		fprintf(dest, "else\n");
		begin_line(dest, depth);
		fprintf(dest, "{\n");
		helse->fprint(dest, depth+1);
		begin_line(dest, depth);
		fprintf(dest, "}\n");
	}
	if (next != 0)
	{
		next->fprint(dest, depth);
	}
}
