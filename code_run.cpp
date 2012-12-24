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

void code_run::fprint(std::ostream *dest, int depth)
{
	unsigned int i;
	for (i = 0; i < els.size(); i++)
	{
		els[i]->fprint(dest, depth);
	}
}