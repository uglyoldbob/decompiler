#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include <cstdint>
#include <string>

class variable
{
	public:
		variable();
		virtual ~variable();
		std::size_t mysize() { return thesize; }
	private:
		char isconst;	//is the datatype constant?
		char sign;	//is the datatype signed (-1), unsigned (1), or irrelevant (0)
		std::string type;
		int num_elements;	//for arrays of specific sizes
		std::size_t thesize;
};

#endif
