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
	begin_line(dest, depth);
	dest << "do\n";
	begin_line(dest, depth);
	dest << "{\n";
	theloop->fprint(dest, depth+1);
	begin_line(dest, depth);
	dest << "} while (?);\n";
}
