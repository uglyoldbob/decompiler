#ifndef __CE_DUMMY_H__
#define __CE_DUMMY_H__

#include "code_element.h"

class ce_dummy : public code_element
{
	public:
		ce_dummy(address addr);
		virtual void get_calls(std::vector<address> &c);	//get a list of function calls
		virtual void fprint(std::ostream &dest, int depth);
	private:
};

#endif
