#include "code_do_while_loop.h"
#include "helpers.h"

code_do_while_loop::code_do_while_loop(code_element *f)
{
	theloop = f;
	s = f->gets();
	copy_inputs(f);
	remove_input(f);
	b = 0;
	if (f->ga() == f)
		a = f->gb();
	else
		a = f->ga();
}

code_do_while_loop::~code_do_while_loop()
{
}

void code_do_while_loop::fprint(std::ostream &dest, int depth)
{
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
	dest << tabs(depth) << "do\n";
	dest << tabs(depth) << "{\n";
	theloop->fprint(dest, depth+1);
	dest << tabs(depth) << "} while (?);\n";
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

void code_do_while_loop::print_graph(std::ostream &dest)
{
	dest << "#do while\n";
	if (theloop != 0)
		theloop->print_graph(dest);
	dest << "#end do while\n";
}
