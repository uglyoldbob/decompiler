#include "code_multi_if.h"
#include "helpers.h"
#include "related_code.h"

/*! \brief Helper class to register code_element creators
 *
 * This class is used to automatically register the code element maker. */
class register_multi_if
{
	public:
		register_multi_if()
		{
			related_code::register_code_element_maker(code_multi_if::simplify);
		}
};

static register_multi_if make_it_so;


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

code_element *code_multi_if::simplify(std::vector<code_element *> grp, code_element *end)
{
	code_multi_if *ret = 0;
	//TODO: make this function do something
	return ret;
}

void code_multi_if::add(code_element *ad)
{
	ifs.push_back(ad);
	if (ifs.size() == 1)
	{
		s = ad->gets();
	}
}

void code_multi_if::common(code_element *c)
{
	common_block = c;
	if (ifs.back()->b != c)
	{
		a = ifs.back()->b;
	}
	else if (ifs.back()->a != c)
	{
		a = ifs.back()->a;
	}
}

void code_multi_if::get_calls(std::vector<address> &c)
{
	for (unsigned int i = 0; i < ifs.size(); i++)
	{
		ifs[i]->get_calls(c);
	}
	if (common_block != 0)
		common_block->get_calls(c);
	if (helse != 0)
		helse->get_calls(c);
	if (thefinal != 0)
		thefinal->get_calls(c);
}

void code_multi_if::fprint(std::ostream &dest, int depth)
{
	unsigned int i;
	ifs[0]->fprint(dest, depth);
	dest << tabs(depth) << "if ( (?)";
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
	dest << tabs(depth) << "{\n";
	common_block->fprint(dest, depth+1);
	dest << tabs(depth) << "}\n";
	if (helse != 0)
	{
		dest << tabs(depth) << "else\n";
		dest << tabs(depth) << "{\n";
		helse->fprint(dest, depth+1);
		dest << tabs(depth) << "}\n";
	}
	
	if (thefinal != 0)
	{
		thefinal->fprint(dest, depth);
	}
}

#ifdef PROVE_SIMPLIFY
void code_multi_if::print_graph(std::ostream &dest)
{
	dest << "#multi if\n";
	for (int i = 0; i < ifs.size(); i++)
	{
		ifs[i]->print_graph(dest);
	}
	if (common_block != 0)
		common_block->print_graph(dest);
	if (helse != 0)
		helse->print_graph(dest);
	if (thefinal != 0)
		thefinal->print_graph(dest);
	dest << "#end multi if\n";
}
#endif

void code_multi_if::set_else(code_element *e)
{
	helse = e;
}

void code_multi_if::set_final(code_element *f)
{
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
