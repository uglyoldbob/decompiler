#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <iostream>
#include <string>

void begin_line(std::ostream &b, int a);
std::string string(int a);
std::string hstring(unsigned int a);

class hex
{
	public:
		hex(int &result);
		friend std::istream &operator>>(std::istream &in, hex a);
	private:
		int &r;
};

class scanset
{
	public:
		scanset(std::string mask, std::string &result);
		scanset(const char *mask, std::string &result);
		friend std::istream &operator>>(std::istream &in, scanset a);
	private:
		std::string m;
		std::string &r;
};

#endif
