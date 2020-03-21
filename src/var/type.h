#ifndef __TYPE__H_
#define __TYPE__H_

#include <string>

/// A class for describing possible variable types.
/** Describes a variable type by name. */
class type
{
	public:
		type(const char *t);
		std::string get_name();
	private:
		std::string name;
};

#endif
