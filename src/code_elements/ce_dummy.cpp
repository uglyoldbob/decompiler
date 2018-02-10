#include "ce_dummy.h"

#include "exceptions.h"
#include "helpers.h"

ce_dummy::ce_dummy(address addr) : code_element(addr)
{
}

void ce_dummy::get_calls(std::vector<address> &c)
{
}

void ce_dummy::fprint(std::ostream &dest, int depth)
{
	dest << tabs(depth) << "#error dummy code (0x" 
		 << std::hex << s << std::dec << ")\n";
}
