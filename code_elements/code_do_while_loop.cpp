#include "code_do_while_loop.h"
#include "helpers.h"

code_do_while_loop::code_do_while_loop(code_element *f)
{
	theloop.push_back(f);
	s = f->gets();
	b = 0;
	if (f->a == f)
		a = f->b;
	else
		a = f->a;
}

code_do_while_loop::code_do_while_loop()
{
}

bool code_do_while_loop::check(code_element *e)
{
	if (e != 0)
	{
		if ((e == e->a) || (e == e->b))
		{
			return true;
		}
	}
	return false;
}

code_element *code_do_while_loop::simplify(std::vector<code_element *> grp, code_element *end)
{
	code_do_while_loop *ret = 0;
	if (grp.size() == 2)
	{
		if (!grp[0]->is_branch())
		{
			if (grp[0]->jumps_to(grp[1]))
			{
				if (grp[1]->branches_to(grp[0]) && grp[1]->branches_to(end))
				{
					ret = new code_do_while_loop();
					ret->theloop.push_back(grp[0]);
					ret->theloop.push_back(grp[1]);
					ret->a = grp[1]->other_branch(grp[0]);
				}
			}
		}
	}
	return ret;
}

code_do_while_loop::~code_do_while_loop()
{
}

void code_do_while_loop::fprint(std::ostream &dest, int depth)
{
	dest << tabs(depth) << "do\n";
	dest << tabs(depth) << "{\n";
	for (unsigned int i = 0; i < theloop.size(); i++)
	{
		theloop[i]->fprint(dest, depth+1);
	}
	dest << tabs(depth) << "} while (?);\n";
}

void code_do_while_loop::get_calls(std::vector<address> &c)
{
	for (unsigned int i = 0; i < theloop.size(); i++)
	{
		theloop[i]->get_calls(c);
	}
}

#ifdef PROVE_SIMPLIFY
void code_do_while_loop::print_graph(std::ostream &dest)
{
	dest << "#do while\n";
	for (unsigned int i = 0; i < theloop.size(); i++)
	{
		theloop[i]->print_graph(dest);
	}
	dest << "#end do while\n";
}
#endif
