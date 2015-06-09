#ifndef __CE_BLOCK_H__
#define __CE_BLOCK_H__

#include <iostream>
#include <vector>

#include "code_element.h"

class ce_block : public code_element
{
	public:
		ce_block(address start);
		~ce_block();
		ce_block* split(address addr);
		instr *getline(int num);	//-1 means get last line
		unsigned int getnline();
		int contains(address addr);
		void fprint(std::ostream &dest, int depth);
		int work(address addr);
		void done();
		int is_done();
		void add_line(instr *addme);
	private:
		ce_block(ce_block* in, address start);
		std::vector<instr*> lines;
		int finished;
};


#endif
