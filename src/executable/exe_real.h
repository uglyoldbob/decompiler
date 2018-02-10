#ifndef __EXE_REAL_H__
#define __EXE_REAL_H__

#include "config.h"
#include "exe_loader.h"

/*
The exe_real class is a class for real executables. This allows
for a dummy class that handles fake or mock executables.
*/
class exe_real : public exe_loader
{
	public:
		exe_real(int reverse);
		~exe_real();
		std::vector<code_element *> gather_instructions(address start_address);
};

#endif
