#include "oper_segbase.h"

oper_segbase::oper_segbase(variable *a, variable *b)
{
	segment = a;
	base = b;
}

std::ostream &oper_segbase::print(std::ostream &out)
{
	if (segment != 0)
	{
		out << *segment;
		if (base != 0)
			out << ":";
	}
	if (base != 0)
	{
		out << *base;
	}
	return out;
}

oper_segbase::~oper_segbase()
{
	delete segment;
	delete base;
}
