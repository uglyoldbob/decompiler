#include "code_while_loop.h"
#include "helpers.h"

code_while_loop::code_while_loop(code_element *st, code_element *loo)
{
	theloop = loo;
	condition = st;
	s = st->gets();
	b = 0;
	if (st->a == loo)
		a = st->b;
	else
		a = st->a;
}

code_while_loop::~code_while_loop()
{
}

void code_while_loop::fprint(std::ostream &dest, int depth)
{
	condition->fprint(dest, depth);
	dest << tabs(depth) << "while (?)\n";
	dest << tabs(depth) << "{\n" << tabs(depth);
	theloop->fprint(dest, depth+1);
	dest << tabs(depth) << "}\n";
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
