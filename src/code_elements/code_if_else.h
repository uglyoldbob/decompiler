#ifndef __CODE_IF_ELSE_H__
#define __CODE_IF_ELSE_H__

#include <iostream>
#include <vector>

#include "code_element.h"

/*! \brief A simple if statement.
 *
 * Simple if statements include a single condition, with an optional else statement.
 */
class code_if_else : public code_element
{
	public:
		code_if_else();
		virtual ~code_if_else();
		void fprint(std::ostream &dest, int depth);
		static code_element *simplify(std::vector<code_element *> grp, code_element *end);
		virtual void get_calls(std::vector<address> &c);	//get a list of function calls
#ifdef PROVE_SIMPLIFY
		virtual void print_graph(std::ostream &dest);
#endif		
	private:
		code_element *condition;
		code_element *hif;	//the logic blocks
		code_element *helse;	//the else block
};

#endif
