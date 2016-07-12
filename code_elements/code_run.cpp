#include "code_run.h"

#include "helpers.h"

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
		s = add->gets();
	}
}

void code_run::done()
{
	a = els.back()->a;
	b = els.back()->b;
}

void code_run::fprint(std::ostream &dest, int depth)
{
	unsigned int i;
	for (i = 0; i < els.size(); i++)
	{
		els[i]->fprint(dest, depth);
	}
}

#ifdef PROVE_SIMPLIFY
void code_run::print_graph(std::ostream &dest)
{
	dest << "#run " << this << "\n";
	for (int i = 0; i < els.size(); i++)
		els[i]->print_graph(dest);
	dest << "#end run\n";
}
#endif
