#ifndef __CODE_MULTI_IF_H__
#define __CODE_MULTI_IF_H__

#include <vector>
#include "code_element.h"

class code_multi_if : public code_element
{
	public:
		code_multi_if();
		~code_multi_if();
	private:
		std::vector<code_element*> ifs;	//the list if conditions that could lead to a common block
		code_element *common_block;
};

#endif