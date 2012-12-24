#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include "config.h"
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <string>

//exception types
	//file failed to open (string filename)
	//unknown executable format (string filename)
	//address not found in executable (address addr)
	//entry address unknown (char *entryname)
	//invalid instruction (address addr)

class file_open_failed : public std::runtime_error
{
	public:
		file_open_failed(const std::string& filename)
			: runtime_error(filename) {}
};

class unknown_exe_format : public std::runtime_error
{
	public:
		unknown_exe_format(const std::string& filename)
			: runtime_error(filename) {}
};

template <class T>

class exception_numerical
{
	public:
		exception_numerical(T const &ref) { data = ref; }
		const T what() const { return data; }
	private:
		T data;	
};

class address_not_present : public exception_numerical<address>
{
	public:
		address_not_present(const address addr)
			: exception_numerical(addr) {}
};

class no_entry_address : public std::runtime_error
{
	public:
		no_entry_address(const std::string& entryname)
			: runtime_error(entryname) {}
};

class invalid_instruction : public exception_numerical<address>
{
	public:
		invalid_instruction(const address addr)
			: exception_numerical(addr) {}
};

#endif