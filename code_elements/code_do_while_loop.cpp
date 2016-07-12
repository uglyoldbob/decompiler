#include "code_do_while_loop.h"
#include "helpers.h"

code_do_while_loop::code_do_while_loop(code_element *f)
{
	theloop = f;
	s = f->gets();
	b = 0;
	if (f->a == f)
		a = f->b;
	else
		a = f->a;
}

bool code_do_while_loop::check(code_element *e)
{
	return false;
}

code_do_while_loop::~code_do_while_loop()
{
}

void code_do_while_loop::fprint(std::ostream &dest, int depth)
{
	dest << tabs(depth) << "do\n";
	dest << tabs(depth) << "{\n";
	theloop->fprint(dest, depth+1);
	dest << tabs(depth) << "} while (?);\n";
}
#ifdef PROVE_SIMPLIFY
void code_do_while_loop::print_graph(std::ostream &dest)
{
	dest << "#do while\n";
	if (theloop != 0)
		theloop->print_graph(dest);
	dest << "#end do while\n";
}
#endif
