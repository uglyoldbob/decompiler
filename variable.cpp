#include "variable.h"

#include <cctype>

variable::variable()
{
	p = OPER_LVL0;
	isconst = 0;
}

variable::variable(std::string in)
{
	p = OPER_LVL0;
	if (isdigit(in[0]))
	{
		isconst = 1;
	}
	else
	{
		isconst = 0;
	}
	sign = 0;
	type = "?";
	name = in;
	num_elements = 0;
	addr = 0;
	valid_address = 0;
	thesize = 0;
}

variable::~variable()
{
}

std::ostream &operator<<(std::ostream &out, variable &o)
{
	return o.print(out);
}

bool variable::needs_trace()
{
	if (isconst==1)
		return 0;
	return 1;
}

variable* variable::trace(variable *trc, code_element *cel, int stmt, int line)
{
	std::cout << "Tracing variable " << *trc << "\n";
	if (this->name == trc->name)
	{
		std::cout << "\tmatch\n";
		return this;
	}
	else
	{
		std::cout << "\tno match\n";
		return 0;
	}
}

oper_precedence variable::get_p()
{
	return p;
}

std::ostream &variable::print(std::ostream &out)
{
	out << name;
	return out;
}
