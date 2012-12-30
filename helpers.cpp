#include "helpers.h"
#include <sstream>

void begin_line(std::ostream &b, int a)
{
	for (int xyz = 0; xyz < a; ++xyz)
	{
		b << "\t";
	}
}

std::string string(int a)
{
	std::stringstream lineout(std::stringstream::in | std::stringstream::out);
	lineout << a;
	std::string retval = lineout.str();
	return retval;
}

std::string hstring(unsigned int a)
{
	std::stringstream lineout(std::stringstream::in | std::stringstream::out);
	lineout << std::hex << a;
	std::string retval = "0x" + lineout.str();
	return retval;
}

scanset::scanset(std::string mask, std::string &result)
	: r(result)
{
	m = mask;
	r.clear();
}

scanset::scanset(const char *mask, std::string &result)
	: r(result)
{
	m = std::string(mask);
	r.clear();
}

std::istream &operator>>(std::istream &in, scanset a)
{
	while (1)
	{
		unsigned char temp;
		temp = in.peek();
		if (a.m[0] == '^')
		{	//match everything except what is in r
			int fail = 0;
			for (unsigned int i = 1; i < a.m.size(); i++)
			{
				if (temp == a.m[i])
				{
					fail = 1;
					break;	//don't read this value in
				}
			}
			if (fail)
				break;
			//letter was not in the reject list
			in >> temp;
			a.r += temp;
		}
		else
		{	//only match what is in r
			int fail = 1;
			for (unsigned int i = 0; i < a.m.size(); i++)
			{
				if (temp == a.m[i])
				{
					in >> temp;
					a.r += temp;
					fail = 0;
					break;	//found a matching letter
				}
			}
			if (fail)
				break;	//character was not in the matching category
		}
	}
	return in;
}

hex::hex(int &result)
	: r(result)
{
}

std::istream &operator>>(std::istream &in, hex a)
{
	unsigned char temp = 0;
	char negative = 0;

	if (in.peek() == ' ')
	{
		while (in.peek() == ' ')
			temp = in.get();
	}

	temp = in.peek();
	if (temp == '-')
	{
		negative = 1;
		temp = in.get();
		temp = in.peek();
	}
	if (temp == '0')
	{
		temp = in.get();
		temp = in.peek();
		if (temp == 'x')
		{	//an actual number
			temp = in.get();
			in >> std::hex >> a.r >> std::dec;
		}
		else
		{	//the value is 0
			a.r = 0;
		}
	}
	else if (isdigit(temp))
	{
		a.r = temp - '0';
	}
	if (negative)
		a.r = -a.r;

	return in;
}
