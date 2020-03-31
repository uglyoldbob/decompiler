#include "code_while_loop.h"
#include "helpers.h"
#include "related_code.h"

/*! \brief Helper class to register code_element creators
 *
 * This class is used to automatically register the code element maker. */
class register_while
{
	public:
		register_while()
		{
			related_code::register_code_element_maker(code_while_loop::simplify);
		}
};

static register_while make_it_so;

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

code_element *code_while_loop::simplify(std::vector<code_element *> grp, code_element *end)
{
	//first item must point to two elements to be an if else
	code_while_loop *ret = 0;
	if (grp[0]->is_branch() && 
		grp[0]->branches_to(end) && 
	 	grp[0]->other_branch(end)->jumps_to(grp[0]) )
	{
		ret = new code_while_loop(grp[0], grp[0]->other_branch(end));		
	}
	return ret;
}

void code_while_loop::get_calls(std::vector<address> &c)
{
	if (condition != 0)
		condition->get_calls(c);
	if (theloop != 0)
		theloop->get_calls(c);
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
