#ifndef __BUILD_SYSTEM_H__
#define __BUILD_SYSTEM_H__

#include "executable/executable.h"

class build_system
{
	public:
		virtual void write_files(std::string output_folder) = 0;
		void add_prog(executable *t);
	protected:
		std::vector<executable *> progs;
};

#endif
