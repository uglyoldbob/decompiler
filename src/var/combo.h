#ifndef __COMBO_H__
#define __COMBO_H__

#include "code_elements/code_element.h"

/// A combination of one or more code_element objects.
/** This class needs help. It is not well defined. A combination of one or more code_element objects. The combo object is equivalent to the combination of all elements it holds. This class is used to reduce the complexity of a map of multiple code_element objects into a single object. */
class combo
{
	public:
		combo(code_element *begin, unsigned int num_el); ///< Initialize the object with a list of code_element objects of the specified number of objects.
		bool valid(); ///< Returns true when all elements are indexed
		std::vector<code_element*> get_combination();
		void next_combo();
	private:
		unsigned int num_elements;	///< The number of code_element objects that are supposed to exist in this object.
		code_element *b; ///< All the code_element objects this combo contains.
		std::vector<unsigned int> indexes; ///< A list of indexes that refer to the b array of code_element objects
		void update_available(std::vector<code_element*> &avail, 
			std::vector<code_element*> grp); ///< This function clears avail, and returns all elements point to by grp that do not exist in grp. This corresponds to references to code_element object to elements outside of what is in grp.
};

#endif
