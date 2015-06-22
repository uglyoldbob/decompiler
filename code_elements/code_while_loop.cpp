#include "code_while_loop.h"
#include "helpers.h"

code_while_loop::code_while_loop(code_element *st, code_element *loo)
{
	theloop = loo;
	condition = st;
	s = st->gets();
	copy_inputs(st);
	remove_input(st);
	b = 0;
	if (st->ga() == loo)
		a = st->gb();
	else
		a = st->ga();
	//st->dins(1);
}

code_while_loop::~code_while_loop()
{
}

void code_while_loop::fprint(std::ostream &dest, int depth)
{
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
	condition->fprint(dest, depth);
	dest << tabs(depth) << "while (?)\n";
	dest << tabs(depth) << "{\n" << tabs(depth);
	theloop->fprint(dest, depth+1);
	dest << tabs(depth) << "}\n";
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
void code_while_loop::print_graph(std::ostream &dest)
{
	dest << "#while loop\n";
	if (condition != 0)
		condition->print_graph(dest);
	if (theloop != 0)
		theloop->print_graph(dest);
	dest << "#end while loop\n";
}
#endif
