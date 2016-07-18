#ifndef __CE_BASIC_H__
#define __CE_BASIC_H__

#include "code_element.h"

class ce_basic : public code_element
{
	public:
		ce_basic(address addr);
		bool should_be_added(address);
		bool no_end();
		int contains(address addr);
		ce_basic* first_half(address addr);
		ce_basic* second_half(address addr);
		void add_line(instr *addme);

		virtual std::vector<address> get_nexts();
		virtual std::vector<address> get_calls();	//get a list of function calls
		
		virtual void fprint(std::ostream &dest, int depth);
	private:
		bool is_branch;	//used to declare this element branches and is a finished block
		std::vector<instr> lines;
};

#endif
