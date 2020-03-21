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

/// Helper class for outputting multiple tabs to an output stream.
/** Convenience class for creating multiple tabs in an output stream. Currently always outputs the specified number of tabs. Future functionality includes outputting the specified number of (spaces) or (2 spaces) or (n spaces). */
class tabs
{
	public:
		tabs(int howmany);
		friend std::ostream &operator<<(std::ostream&out, tabs a);
	private:
		int hm;
};

#endif
