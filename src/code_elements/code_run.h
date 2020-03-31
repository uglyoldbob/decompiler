#ifndef __CODE_RUN_H__
#define __CODE_RUN_H__

#include "code_element.h"
#include <iostream>
#include <vector>

/*! \brief A group of sequential code_element objects.
 *
 * These code_elements are simply chunks of code that run in a row.
 */
class code_run : public code_element
{
	public:
		code_run();
		~code_run();
		static code_element *simplify(std::vector<code_element *> grp, code_element *end);
		void add_element(code_element *add);
		void done();
		void fprint(std::ostream &dest, int depth);
		virtual void get_calls(std::vector<address> &c);
#ifdef PROVE_SIMPLIFY
		virtual void print_graph(std::ostream &dest);
#endif
	private:
		std::vector<code_element *> els;	///< The string of elements
};

#endif
