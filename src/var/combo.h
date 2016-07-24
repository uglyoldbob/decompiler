#ifndef __COMBO_H__
#define __COMBO_H__

#include "code_elements/code_element.h"

class combo
{
	public:
		combo(code_element *begin, unsigned int num_el);
		bool valid();
		std::vector<code_element*> get_combination();
		void next_combo();
	private:
		unsigned int num_elements;
		code_element *b;
		std::vector<unsigned int> indexes;
		void update_available(std::vector<code_element*> &avail, 
			std::vector<code_element*> grp);
};

#endif
