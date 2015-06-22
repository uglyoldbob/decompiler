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
		copy_inputs(add);
		s = add->gets();
	}
}

void code_run::done()
{
	a = els.back()->ga();
	b = els.back()->gb();
}

void code_run::fprint(std::ostream &dest, int depth)
{
	unsigned int i;
	/*dest << tabs(depth) << "/------";
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
	dest << "\n";*/
	for (i = 0; i < els.size(); i++)
	{
		els[i]->fprint(dest, depth);
	}
	/*dest << tabs(depth) << "\\------ ";
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
	dest << "\n";*/
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
