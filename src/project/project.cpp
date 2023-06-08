#include <cstring>
#include <sys/stat.h>
#include "project.h"

project::project(build_system *bs) : bld(bs)
{
}

void project::set_output_dir(const char *n)
{
	std::string temp(n);
	dir = temp;
	
	int status = mkdir(dir.c_str());
	if (status == -1)
	{
		switch (errno)
		{
		case EEXIST:
			break;
		default:
			std::cout << "Status: " << status << " " << errno << std::endl;
			throw "Could not create output folder";
			break;
		}	
	}
}

void project::add_program(executable *t)
{
	exes.push_back(t);
	if (bld != 0)
	{
		bld->add_prog(t);
	}
}

void project::write_build_system()
{
	if (bld != 0)
	{
		bld->write_files(dir);
	}
}

void project::write_sources()
{
	for(unsigned int i = 0; i < exes.size(); i++)
	{
		exes[i]->write_sources(dir);
	}
}

