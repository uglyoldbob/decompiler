#ifndef __CE_BLOCK_H__
#define __CE_BLOCK_H__

#include <iostream>
#include <vector>

#include "code_element.h"

class ce_block : public code_element
{
	public:
		ce_block();
		ce_block(const ce_block &src, address start, address end);
		~ce_block();
		instr *getline(int num);	//-1 means get last line
		unsigned int getnline();
		void fprint(std::ostream &dest, int depth);
		void work(address addr);
		void done();
		int is_done();
	private:
		std:: vector<instr*> lines;
		int finished;
};


#endif
