#ifndef __CE_BASIC_H__
#define __CE_BASIC_H__

#include <memory>

#include "code_element.h"

/// Basic storage element for multiple instructions
/** This object is generally one of the first steps in decompilation after instruction objects are created. This class is used to put together the raw execution flow map of the code being decompiled. */
class ce_basic : public code_element
{
	public:
		ce_basic(address addr); ///< Creates a basic code block starting at the specified address
		bool should_be_added(address); ///< Determines if the instruction at the specified address should be added to this block.
		int contains(address addr); ///< Determines if the specified address currently exists in this code block. Returns -1 if it does, 0 if it does not
		ce_basic* first_half(address addr); ///< Splits the block into two groups using the specified address. Returns the first element of two.
		ce_basic* second_half(address addr); ///< Splits the block into two groups using the specified address. Returns the second element of two.
		void add_line(instr *addme);	///< add an instruction to the end of this block of code

		virtual std::vector<address> get_nexts(); ///< Returns a vector of addresses this block of code might branch or jump or whatever to start executing
		virtual void get_calls(std::vector<address> &c);	///<get a list of function calls
		
		virtual void fprint(std::ostream &dest, int depth); ///< Print all lines of code to the specified output stream. Outputs c style code. Depth specifies how deep to indent every line
	private:
		bool is_branch;	///< Used to declare this element branches and is a finished block. A block that branches is finished.
		std::vector<instr> lines; ///< Vector of all instructions
};

#endif
