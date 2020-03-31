#ifndef __CODE_WHILE_LOOP_H__
#define __CODE_WHILE_LOOP_H__

#include "code_element.h"
#include <iostream>

/*! \brief A while loop.
 *
 * Uses a set of code_elements to represent a while loop.
 */
class code_while_loop : public code_element
{
	public:
		/*! Create a while loop with the specified elements. 
		 * @param f The condition for the while loop
		 * @param g The body of the loop
		 */
		code_while_loop(code_element *f, code_element *g);
		~code_while_loop();
		void fprint(std::ostream &dest, int depth);
		static code_element *simplify(std::vector<code_element *> grp, code_element *end);
		virtual void get_calls(std::vector<address> &c);	//get a list of function calls
#ifdef PROVE_SIMPLIFY
		virtual void print_graph(std::ostream &dest);
#endif
	private:
		code_element *condition; ///< The conditional statement of the while loop.
		code_element *theloop; ///< The body of the while loop.
};

#endif
