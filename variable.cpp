#include "variable.h"

#include <cctype>
#include "helpers.h"

variable::variable()
{
	p = OPER_LVL0;
	isconst = 0;
}

variable::variable(std::string in, std::size_t size)
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
	sign = VAR_SIGN_SIGNED | VAR_SIGN_UNSIGNED;
	type = "?";
	name = in;
	num_elements = 0;
	addr = 0;
	valid_address = 0;
	thesize = size;
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

variable* variable::trace(int d, variable *trc, code_element *cel, int stmt, int line)
{
	if (this->name == trc->name)
	{
		return this;
	}
	else
	{
		return 0;
	}
}

oper_precedence variable::get_p()
{
	return p;
}

std::size_t variable::get_size()
{
	if (thesize & VAR_SIZE_OTHER)
		return othersize;
	else if (thesize & VAR_SIZE_64_BITS)
		return 8;
	else if (thesize & VAR_SIZE_32_BITS)
		return 4;
	else if (thesize & VAR_SIZE_16_BITS)
		return 2;
	else if (thesize & VAR_SIZE_8_BITS)
		return 1;
	return 0;
}

std::string variable::get_name()
{
	return name;
}

std::ostream &variable::print(std::ostream &out)
{
	out << name;
	return out;
}
