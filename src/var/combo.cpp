#include "combo.h"

#include "code_elements/related_code.h"

combo::combo(code_element *begin, unsigned int num_el)
{
	std::vector<code_element*> used;
	std::vector<code_element*> avail;
	num_elements = num_el;
	//indexes refers to the index of the avail vector
	//used refers to the elements already used
	//avail refers to the elements that are unused and available
	//avail is generated from the "used" vector

	b = begin;
	
	avail.push_back(b);
	
	while ((indexes.size() < num_elements) && (avail.size() > 0))
	{
		indexes.push_back(0);
		used.push_back(avail[0]);
		update_available(avail, used);
	}
}

bool combo::valid()
{
	return (num_elements == indexes.size());
}

void combo::update_available(std::vector<code_element*> &avail, 
			std::vector<code_element*> grp)
{
	avail.clear();
	for (unsigned int i = 0; i < grp.size(); i++)
	{
		if (grp[i]->a != 0)
		{
			if ( !element_present(grp, grp[i]->a->gets()) &&
				 !element_present(avail, grp[i]->a->gets())
				)
			{
				avail.push_back(grp[i]->a);
			}
		}
		if (grp[i]->b != 0)
		{
			if ( !element_present(grp, grp[i]->b->gets()) &&
				 !element_present(avail, grp[i]->b->gets())
				)
			{
				avail.push_back(grp[i]->b);
			}
		}
	}
}

std::vector<code_element*> combo::get_combination()
{
	std::vector<code_element*> used;
	std::vector<code_element*> avail;
	//indexes refers to the index of the avail vector
	//used refers to the elements already used
	//avail refers to the elements that are unused and available
	//avail is generated from the "used" vector
	
	avail.push_back(b);
	
	while ((used.size() < num_elements) && (avail.size() > 0))
	{
		used.push_back(avail[indexes[used.size()]]);
		update_available(avail, used);
	}
	
	return used;
}

void combo::next_combo()
{
	std::vector<code_element*> used;
	std::vector<code_element*> avail;
	//indexes refers to the index of the avail vector
	//used refers to the elements already used
	//avail refers to the elements that are unused and available
	//avail is generated from the "used" vector
	
	int target_index = num_elements - 1;
	
	while (target_index >= 0)
	{
		avail.clear();
		avail.push_back(b);
		used.clear();
		while ((used.size() < target_index) && (avail.size() > 0))
		{
			used.push_back(avail[indexes[used.size()]]);
			update_available(avail, used);
		}
		indexes[target_index]++;
		if (indexes[target_index] < avail.size())
		{
			used.push_back(avail[indexes[target_index]]);
			update_available(avail, used);
			target_index = -1;
		}
		else if (target_index > 0)
		{
			
			indexes[target_index] = 0;
			target_index--;
		}
		else
		{
			indexes.clear();
			target_index = -1;
		}
	}
}
