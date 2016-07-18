#include "source_file.h"

source_file::source_file(std::string s) : name(s)
{
}

source_file::~source_file()
{
	for (unsigned int i = 0; i < funcs.size(); i++)
		delete funcs[i];
}

std::string source_file::get_name()
{
	return name;
}

void source_file::add_function(function* f)
{
	funcs.push_back(f);
}

bool source_file::find_function(address a)
{
	for (unsigned int i = 0; i < funcs.size(); i++)
	{
		if (funcs[i]->gets() == a)
		{
			return true;
		}
	}
	return false;
}

std::vector<address> source_file::get_calls()
{
	std::vector<address> ret;
	for (unsigned int i = 0; i < funcs.size(); i++)
	{
		std::vector<address> t = funcs[i]->get_calls();
		for (unsigned int j = 0; j < t.size(); j++)
		{
			ret.push_back(t[j]);
		}
	}
	return ret;
}

void source_file::write_sources(std::string n)
{
	std::string fname(n + "/" + name);
	std::ofstream out(fname.c_str(), std::ios::out);
	for (unsigned int i = 0; i < funcs.size(); i++)
	{
		out << *(funcs[i]);
	}
}

