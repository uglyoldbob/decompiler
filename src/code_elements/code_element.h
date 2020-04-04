#ifndef __CODE_ELEMENT_H__
#define __CODE_ELEMENT_H__

#include "config.h"
#include "disassembly/disassembler.h"
#include <iostream>
#include <vector>

#define PROVE_SIMPLIFY 1
#undef PROVE_SIMPLIFY

/*! \brief A basic block of code.
 *
 *
 */
class code_element
{
	public:
		code_element(address addr); ///< Create a block of code that starts at the specified address.
		code_element();
		virtual ~code_element();
		
		address gets();		///< Return the starting address of the code_element
		virtual std::vector<address> get_nexts(); ///< A list of the addresses this code_element may execute after itself
		bool is_branch();	///< Returns true if this code element is a branching type (has more than one next address)
		bool jumps_to(code_element *m); ///< Returns true if this code_element jumps to the specified element.
		bool branches_to(code_element *m); ///< Returns true if this code_element branches to the specified element.
		bool dead_end(); ///< Returns true if this code_element goes nowhere.
		code_element *other_branch(code_element *m); ///< Returns either a or b, whichever does not match.
		
		virtual void get_calls(std::vector<address> &c) = 0;	///< Returns a list of function calls this code_element makes

		void replace_references(code_element *old, code_element *nw); ///< Replace all references of an old object with a new one
		virtual void fprint(std::ostream &dest, int depth) = 0; ///< Print the code for the code_element
		virtual void print_graph(std::ostream &dest); ///< Output data for a graphviz graph
		
		code_element *a;	///< One of the next elements
		code_element *b;	///< The other next element
	protected:
		address s;	///< The starting address of the code_element
};

#endif
