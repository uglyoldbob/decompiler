#include "code_while_loop.h"

code_while_loop::code_while_loop(code_element *st, code_element *loo)
{
	theloop = loo;
	condition = st;
	s = st->gets();
	ins = st->gins()-1;
	b = 0;
	if (st->ga() == loo)
		a = st->gb();
	else
		a = st->ga();
	st->dins(1);
}

code_while_loop::~code_while_loop()
{
}

void code_while_loop::fprint(std::ostream *dest, int depth)
{
	condition->fprint(dest, depth);
	begin_line(dest, depth);
	*dest << "while (?)\n";
	begin_line(dest, depth);
	*dest << "{\n";
	begin_line(dest, depth);
	theloop->fprint(dest, depth+1);
	begin_line(dest, depth);
	*dest << "}\n";
}