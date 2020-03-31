#include "related_code.h"
#include "code_if_else.h"
#include "helpers.h"

/*! \brief Helper class to register code_element creators
 *
 * This class is used to automatically register the code element maker. */
class register_if_else
{
	public:
		register_if_else()
		{
			related_code::register_code_element_maker(code_if_else::simplify);
		}
};

static register_if_else make_it_so;

code_if_else::code_if_else()
{
	condition = NULL;
	hif = NULL;
	helse = NULL;
}

code_if_else::~code_if_else()
{
}

code_element *code_if_else::simplify(std::vector<code_element *> grp, code_element *end)
{
	//first item must point to two elements to be an if else
	code_if_else *ret = 0;
	if (grp[0]->is_branch())
	{
		if ((grp.size() == 2) && (grp[0]->branches_to(end)) && (grp[0]->other_branch(end)->jumps_to(end)))
		{	//possibly just an if when one element points to *end
			ret = new code_if_else();
			ret->condition = grp[0];
			ret->hif = grp[0]->other_branch(end);
			ret->helse = 0;
			ret->s = ret->condition->gets();
			ret->a = end;
			ret->b = 0;
		}
		else if ((grp.size() == 3) && grp[0]->a->jumps_to(end) && grp[0]->b->jumps_to(end))
		{
			ret = new code_if_else();
			ret->condition = grp[0];
			ret->hif = grp[0]->a;
			ret->helse = grp[0]->b;
			ret->s = ret->condition->gets();
			ret->a = end;
			ret->b = 0;
		}
		else if ((grp.size() == 3) && grp[0]->a->dead_end() && grp[0]->b->dead_end())
		{
			ret = new code_if_else();
			ret->condition = grp[0];
			ret->hif = grp[0]->a;
			ret->helse = grp[0]->b;
			ret->s = ret->condition->gets();
			ret->a = 0;
			ret->b = 0;
		}
	}
	
	return ret;
}

void code_if_else::get_calls(std::vector<address> &c)
{
	if (condition != 0)
		condition->get_calls(c);
	if (hif != 0)
		hif->get_calls(c);
	if (helse != 0)
		helse->get_calls(c);
}

void code_if_else::fprint(std::ostream &dest, int depth)
{
	unsigned int i;
	condition->fprint(dest, depth);
	dest << tabs(depth) << "if (?)\n";
	dest << tabs(depth) << "{\n";
	hif->fprint(dest, depth+1);
	dest << tabs(depth) << "}\n";
	if (helse != 0)
	{
		dest << tabs(depth) << "else\n";
		dest << tabs(depth) << "{\n";
		helse->fprint(dest, depth+1);
		dest << tabs(depth) << "}\n";
	}
}

#ifdef PROVE_SIMPLIFY
void code_if_else::print_graph(std::ostream &dest)
{
	dest << "#if else\n";
	condition->print_graph(dest);
	hif->print_graph(dest);
	if (helse != 0)
		helse->print_graph(dest);
	dest << "#end if else\n";
}
#endif
