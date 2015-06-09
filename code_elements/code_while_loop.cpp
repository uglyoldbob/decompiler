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
	condition->fprint(dest, depth);
	dest << tabs(depth) << "while (?)\n";
	dest << tabs(depth) << "{\n" << tabs(depth);
	theloop->fprint(dest, depth+1);
	dest << tabs(depth) << "}\n";
}
