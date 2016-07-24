#include "related_code_maker.h"

related_code_maker::related_code_maker()
{
}

void related_code_maker::set_num_elements(unsigned int n)
{
	num_elements = n;
	for (unsigned int i = 0; i < group.size(); i++)
	{
		delete group[i];
	}
	group.clear();
	indexes.clear();
	for (unsigned int i = 0; i < n; i++)
	{
		group.push_back(new ce_dummy(i+1));
		indexes.push_back(0);
		indexes.push_back(0);
	}
	rc = 0;
}

void related_code_maker::update()
{
	if (rc != 0)
		delete rc;
	rc = new related_code();
	for (unsigned int i = 0; i < group.size(); i++)
	{
		if (indexes[2*i] == 0)
		{
			group[i]->a = 0;
		}
		else
		{
			group[i]->a = group[indexes[2*i]-1];
		}
		if (indexes[2*i+1] == 0)
		{
			group[i]->b = 0;
		}
		else
		{
			group[i]->b = group[indexes[2*i+1]-1];
		} 
	}
	for (unsigned int i = 0; i < group.size(); i++)
	{
		rc->add_block(group[i]);
	}
}

void related_code_maker::simplify()
{
	update();
	rc->simplify();
}

bool related_code_maker::next()
{
	bool bad_combo;
	bool done;
	do
	{
		unsigned int i = 0;
		bad_combo = false;
		done = false;
		while ((!done) && (i < group.size()))
		{
			indexes[i]++;
			if (indexes[i] > group.size())
			{
				indexes[i] = 0;
				i++;
			}
			else
			{
				done = true;
			}
			update();
			for (unsigned int j = 1; j < group.size(); j++)
			{
				if (!non_self_reference(group, group[j]->gets()))
					bad_combo = true;
			}
			int num_deads = 0;
			for (unsigned int j = 0; j < group.size(); j++)
			{
				if ((group[j]->a == group[j]->b) && (group[j]->a != 0))
					bad_combo = true;
				if ( (group[j]->a == 0) && (group[j]->b != 0))
					bad_combo = true;
				if ( (group[j]->a == 0) && (group[j]->b == 0))
					num_deads++;
			}
			if (num_deads == 0)
			{
				bad_combo = true;
			}
		}
	} while (bad_combo && done);
	return !bad_combo;
}

bool related_code_maker::simplified()
{
	return rc->simplified();
}

related_code *related_code_maker::get_rc()
{
	return rc;
}
