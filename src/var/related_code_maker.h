#ifndef __RELATED_CODE_MAKER_H__
#define __RELATED_CODE_MAKER_H__

#include "code_elements/ce_dummy.h"
#include "code_elements/related_code.h"

/// A container for related code_element objects.
/** This class needs help. It is not well defined. */
class related_code_maker
{
	public:
		related_code_maker();
		void set_num_elements(unsigned int n);
		void simplify();
		bool simplified();
		bool next();
		related_code *get_rc();
		void update();
	private:
		int num_elements;
		std::vector<code_element*> group;
		std::vector<unsigned int> indexes;
		related_code *rc;
		
		
};

#endif
