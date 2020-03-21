#include "statement.h"

#include <cctype>
#include "helpers.h"

statement::statement() : var_type("void")
{
	p = OPER_LVL0;
	isconst = 0;
}

statement::statement(std::string in, std::size_t size) : var_type("void")
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
	name = in;
	num_elements = 0;
	addr = 0;
	valid_address = 0;
	thesize = size;
}

statement::~statement()
{
}

std::ostream &operator<<(std::ostream &out, statement &o)
{
	return o.print(out);
}

oper_precedence statement::get_p()
{
	return p;
}

std::size_t statement::get_size()
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

std::string statement::get_name()
{
	return name;
}

std::ostream &statement::print(std::ostream &out)
{
	out << name;
	return out;
}
