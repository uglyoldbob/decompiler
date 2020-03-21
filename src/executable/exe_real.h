#ifndef __EXE_REAL_H__
#define __EXE_REAL_H__

#include "config.h"
#include "exe_loader.h"

/// For real executables.
/** The exe_real class is a class for real executables. This allows for a dummy class that handles fake or mock executables. It provides a generic implementation of gather_instructions */
class exe_real : public exe_loader
{
	public:
		exe_real(int reverse);
		~exe_real();
		std::vector<code_element *> gather_instructions(address start_address);
};

#endif
