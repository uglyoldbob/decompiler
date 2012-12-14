#include "code_run.h"

code_run::code_run()
{
}

code_run::~code_run()
{
}

void code_run::add_element(code_element *add)
{
	els.push_back(add);
	if (els.size() == 1)
	{
		ins = add->gins();
		s = add->gets();
	}
}

void code_run::done()
{
	a = els.back()->ga();
	b = els.back()->gb();
}

void code_run::fprint(FILE *dest, int depth)
{
	unsigned int i;
	fprintf(dest, "start of a run %d\n", els.size());
	for (i = 0; i < els.size(); i++)
	{
		fprintf(dest, "run %d of %d\n", i+1, els.size());
		els[i]->fprint(dest, depth);
	}
}