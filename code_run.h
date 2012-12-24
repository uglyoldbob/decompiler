#ifndef __CODE_RUN_H__
#define __CODE_RUN_H__

#include "code_element.h"
#include <iostream>
#include <vector>

class code_run : public code_element
{
	public:
		code_run();
		~code_run();
		void add_element(code_element *add);
		void done();
		void fprint(std::ostream &dest, int depth);
	private:
		std::vector<code_element *> els;	//a string of elements
};

#endif
