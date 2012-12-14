#include "code_multi_if.h"

code_multi_if::code_multi_if()
{
	form = NONE;
	helse = 0;
	thefinal = 0;
}

code_multi_if::~code_multi_if()
{
	form = NONE;
}

void code_multi_if::add(code_element *ad)
{
	ifs.push_back(ad);
	if (ifs.size() == 1)
	{
		ins = ad->gins();
		s = ad->gets();
	}
}

void code_multi_if::common(code_element *c)
{
	common_block = c;
	if (ifs.back()->gb() != c)
	{
		a = ifs.back()->gb();
	}
	else if (ifs.back()->ga() != c)
	{
		a = ifs.back()->ga();
	}

/*	if (c->ga() != a)
	{
		b = c->ga();
	}
	else
	{
		c->ga()->dins(1);
	}*/
}

void code_multi_if::fprint(FILE *dest, int depth)
{
	unsigned int i;
	ifs[0]->fprint(dest, depth);
	begin_line(dest, depth);
	fprintf(dest, "if ( (?)");
	for (i = 1; i < ifs.size(); i++)
	{
		fprintf(dest, " || (?)");
	}
	fprintf(dest, " )\n");
	begin_line(dest, depth);
	fprintf(dest, "{\n");
	common_block->fprint(dest, depth+1);
	begin_line(dest, depth);
	fprintf(dest, "}\n");
	if (helse != 0)
	{
		begin_line(dest, depth);
		fprintf(dest, "else\n");
		begin_line(dest, depth);
		fprintf(dest, "{\n");
		helse->fprint(dest, depth+1);
		begin_line(dest, depth);
		fprintf(dest, "}\n");
	}
	
	if (thefinal != 0)
	{
		thefinal->fprint(dest, depth);
	}
}

void code_multi_if::set_else(code_element *e)
{
	helse = e;
}

void code_multi_if::set_final(code_element *f)
{
	printf("Setting final to %x (%d)\n", (int)f->gets(), f->gins());
	if (f->gins() == 1)
	{
		printf("\tabsorbed\n");
		thefinal = f;
		a = f->ga();
		b = f->gb();
	}
	else if (f->gins() > 1)
	{
		printf("\tnot absorbed %x %d\n", f->gets(), f->gins());
		a = f;
		b = 0;
	}
}

void code_multi_if::finish_and_no_else()
{
	printf("Finishing multi-AND with NO ELSE\n");
	form = AND_NO_ELSE;
}

void code_multi_if::finish_or_no_else()
{
	printf("Finishing multi-OR with NO ELSE\n");
	form = OR_NO_ELSE;
	
}

void code_multi_if::finish_with_else()
{
	printf("Finishing multi-AND/OR with ELSE\n");
	form = WITH_ELSE;
	
}
