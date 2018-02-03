#include "code_run.h"

#include "related_code.h"
#include "helpers.h"

code_run::code_run()
{
}

code_run::~code_run()
{
}

code_element *code_run::simplify(std::vector<code_element *> grp, code_element *end)
{
	code_run *ret = 0;
	//all elements must be non-branching
	bool all_nonbranch = true;
	for (unsigned int i = 0; i < grp.size(); i++)
	{
		if (grp[i]->is_branch())
		{
			all_nonbranch = false;
		}
	}
	if (grp.size() == 1)
	{
		all_nonbranch = false;
	}
	if (all_nonbranch)
	{
		ret = new code_run();
		code_element *temp = grp[0];
		do
		{
			ret->add_element(temp);
			temp = temp->a;
		} while (temp != end);
		ret->a = end;
		ret->b = 0;
		std::vector<bool>grp_elements_used;
		for (unsigned int i = 0; i < grp.size(); i++)
		{
			grp_elements_used.push_back(false);
		}
		for (unsigned int i = 0; i < ret->els.size(); i++)
		{
			unsigned int temp = get_index(grp, ret->els[i]);
			if (temp < grp.size())
			{
				grp_elements_used[temp] = true;
			}
		}
		for (unsigned int i = 0; i < grp_elements_used.size(); i++)
		{
			if (!grp_elements_used[i])
			{
				delete ret;
				ret = 0;
			}
		}
	}
	return ret;
}

void code_run::get_calls(std::vector<address> &c)
{
	for (unsigned int i = 0; i < els.size(); i++)
	{
		els[i]->get_calls(c);
	}
}

void code_run::add_element(code_element *add)
{
	els.push_back(add);
	if (els.size() == 1)
	{
		s = add->gets();
	}
}

void code_run::done()
{
	a = els.back()->a;
	b = els.back()->b;
}

void code_run::fprint(std::ostream &dest, int depth)
{
	unsigned int i;
	for (i = 0; i < els.size(); i++)
	{
		els[i]->fprint(dest, depth);
	}
}

#ifdef PROVE_SIMPLIFY
void code_run::print_graph(std::ostream &dest)
{
	dest << "#run " << this->gets() << "\n";
	for (int i = 0; i < els.size(); i++)
		els[i]->print_graph(dest);
	dest << "#end run\n";
}
#endif
