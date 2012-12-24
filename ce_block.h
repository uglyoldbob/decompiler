#ifndef __CE_BLOCK_H__
#define __CE_BLOCK_H__

#include <iostream>

#include "code_element.h"

class ce_block : public code_element
{
	public:
		ce_block();
		~ce_block();
		void setnline(int num);
		int getnline();
		void setline(line_info *a);
		line_info *getline(int num);	//-1 means get last line
		void fprint(std::ostream &dest, int depth);
	private:
		struct line_info **line;
		int num_lines;
};


#endif