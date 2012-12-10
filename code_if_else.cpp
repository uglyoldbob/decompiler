#include "code_if_else.h"

code_if_else::code_if_else()
{
	helse = NULL;
	last = NULL;
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
	last = l;
	a = l->ga();
	b = l->gb();
}

void code_if_else::fprint(FILE *dest, int depth)
{
	unsigned int i;
	int xyz;
	lcb[0]->fprint(dest, depth);
	for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
	fprintf(dest, "if ()\n");
	for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
	fprintf(dest, "{\n");
	ecb[0]->fprint(dest, depth+1);
	for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
	fprintf(dest, "}\n");
	for (i = 1; i < lcb.size(); i++)
	{
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "else if (?)\n{");
		ecb[i]->fprint(dest, depth+1);
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "}\n");
	}
	if (helse != 0)
	{
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "else\n");
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "{\n");
		helse->fprint(dest, depth+1);
		for (xyz = 0; xyz < depth; xyz++) { fprintf(dest, "\t");}
		fprintf(dest, "}\n");
	}
	last->fprint(dest, depth);
}
