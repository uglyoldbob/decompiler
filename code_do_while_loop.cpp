#include "code_do_while_loop.h"

code_do_while_loop::code_do_while_loop(code_element *f)
{
	theloop = f;
	s = f->gets();
	b = 0;
	if (f->ga() == f)
		a = f->gb();
	else
		a = f->ga();
}

code_do_while_loop::~code_do_while_loop()
{
}

void code_do_while_loop::fprint(FILE *dest, int depth)
{
	begin_line(dest, depth);
	fprintf(dest, "do\n");
	begin_line(dest, depth);
	fprintf(dest, "{\n");
	begin_line(dest, depth);
	theloop->fprint(dest, depth+1);
	begin_line(dest, depth);
	fprintf(dest, "} while (?)\n");
}