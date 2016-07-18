#ifndef __AUTOTOOLS_H__
#define __AUTOTOOLS_H__

#include <string>
#include <vector>
#include "build_system.h"
#include "executable/executable.h"

class autotools : public build_system
{
	public:
		virtual void write_files(std::string output_folder);
	private:
		void automake_output(std::string fname);
		void other_output(std::string of);
};

#endif
