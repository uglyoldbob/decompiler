#include "type.h"

type::type(const char *t) : name(t) 
{
}

std::string type::get_name()
{
	return name;
}
