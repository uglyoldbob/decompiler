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

void code_multi_if::fprint(std::ostream &dest, int depth)
{
	unsigned int i;
	ifs[0]->fprint(dest, depth);
	begin_line(dest, depth);
	dest << "if ( (?)";
	if (form == AND_NO_ELSE)
	{
		for (i = 1; i < ifs.size(); i++)
		{
			dest << " && (?)";
		}
	}
	else
	{
		for (i = 1; i < ifs.size(); i++)
		{
			dest << " || (?)";
		}
	}
	dest << " )\n";
	begin_line(dest, depth);
	dest << "{\n";
	common_block->fprint(dest, depth+1);
	begin_line(dest, depth);
	dest << "}\n";
	if (helse != 0)
	{
		begin_line(dest, depth);
		dest << "else\n";
		begin_line(dest, depth);
		dest << "{\n";
		helse->fprint(dest, depth+1);
		begin_line(dest, depth);
		dest << "}\n";
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
	if (f->gins() == 1)
	{
		thefinal = f;
		a = f->ga();
		b = f->gb();
	}
	else if (f->gins() > 1)
	{
		a = f;
		b = 0;
	}
}

void code_multi_if::finish_and_no_else()
{
	form = AND_NO_ELSE;
}

void code_multi_if::finish_or_no_else()
{
	form = OR_NO_ELSE;
}

void code_multi_if::finish_with_else()
{
	form = WITH_ELSE;
}
