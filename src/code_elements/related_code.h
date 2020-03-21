#ifndef __RELATED_CODE_H__
#define __RELATED_CODE_H__

#include <vector>

#include "disassembly/disassembler.h"
#include "code_element.h"

typedef code_element * (*code_element_maker)(std::vector<code_element *> grp, code_element *end);

std::vector<unsigned int> make_combination(int num);
std::vector<code_element *> make_group(int num);
unsigned int get_index(std::vector<code_element*> gr, code_element *b);
bool element_present(std::vector<code_element*> gr, address a);
bool reference_present(std::vector<code_element*> gr, address a);
bool non_self_reference(std::vector<code_element*> gr, address a);

/// A group of code related because execution flows between all the contained elements in some fashion.
/** This class needs help. It is not well specified. The class is used to group code into multiple blocks, which are eventually simplified down to a single block. */
class related_code
{
	public:
		related_code(std::vector<code_element *> a); ///< Create related_code with a list of code_element objects.
		address start_address; ///< The starting address of the related_code. All related_code objects have a singly entry point.
		void add_block(code_element *c); ///< Add the specified single code_element
		void get_calls(std::vector<address> &c);	///< Retrieve a list of all addresses used as functions
		void fprint(std::ostream &dest, int depth); ///< Print all blocks to the given output stream. Include an error that prevents compilation if the elements are not fully simplified.
		void print_graph(std::ostream &dest); ///< Write a graphviz node chart to the specified output stream.
		void simplify(); ///< Atempt to simplify the blocks currently present.
		bool simplified(); ///< Returns true if the code is fully simplified.
		static void register_code_element_maker(code_element_maker a); ///< Register an object that creates code_elements
		static void list_code_element_makers(std::ostream &dest); ///< Output all registered code_element_makers to the given otuput stream.
	private:
		std::vector<code_element *> blocks;	//The basic elements of code
		void finalize_blocks(); ///< Update all blocks with their correct destinations.
		code_element *get_block(address a);	///< Return the block starting with address a
		void replace_element(code_element* old, code_element *n); ///< Replace a code_element with another, including references.
		void replace_group(std::vector<code_element*>a, code_element *b); ///< Replace a group of elements with a single element
		std::vector<code_element *> outside_references(std::vector<code_element *> gr); ///< Returns a list of all references outside the given set of code_element objects
		std::vector<code_element *> external_inputs(std::vector<code_element *> gr); ///< Return a list of all outside elements that point to any element in the list of elements gr.
		void apply_combination(std::vector<unsigned int> cmb, std::vector<code_element *> &gr);
		bool next_combo(std::vector<unsigned int> &cmb);
		int process_blocks(int n);
		
		//A list of functions that can make code_element*
		static std::vector<code_element_maker> *rc_makers;
};

#endif
