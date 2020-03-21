#include "autotools.h"
#include "project/source_file.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>

void autotools::write_files(std::string output_folder)
{
	std::string fname(output_folder + "/configure.ac");
	std::ofstream configure(fname.c_str(), std::ios::out);
	
	configure << "AC_PREREQ([2.62])\n";
	configure << "AC_INIT([project], [0.1])\n";
	configure << "AM_INIT_AUTOMAKE\n";
	configure << "AC_PROG_CC\n";
	configure << "AC_CONFIG_HEADER([ac_config.h:config.h.in])\n";
	configure << "AC_CHECK_TOOL([STRIP],[strip])\n";
	
	configure << "AC_CONFIG_FILES([\n"
		<< "	Makefile\n"
		<< "])\n"
		<< "AC_OUTPUT\n";

	std::string am_name(output_folder + "/Makefile.am");
	automake_output(am_name);
	other_output(output_folder);
}

void autotools::automake_output(std::string fname)
{
	std::ofstream out(fname.c_str(), std::ios::out);
	out << "AUTOMAKE_OPTIONS = subdir-objects\n" << std::flush;
	
	if (progs.size() > 0)
	{
		out << "bin_PROGRAMS =";
		for (unsigned int i = 0; i < progs.size(); i++)
		{
			out << " " << progs[i]->get_name();
		}
		out << "\n" << std::flush;
	}
	
	if (progs.size() > 0)
	{
		for (unsigned int i = 0; i < progs.size(); i++)
		{
			std::vector<source_file*> s;
			s = progs[i]->get_sources();
			if (s.size() > 0)
			{
				out << progs[i]->get_name() << "_SOURCES =";
				for (unsigned int j = 0; j < s.size(); j++)
				{
					out << " \\\n " << s[j]->get_name();
				}
				out << "\n" << std::flush;
			}
		}
	}
}

void autotools::other_output(std::string of)
{
	std::unique_ptr<std::ofstream> out;
	
	std::string news_name(of + "/NEWS");
	out = std::unique_ptr<std::ofstream>(new std::ofstream(news_name.c_str(), std::ios::out));
	(*out) << "\n" << std::flush;
	out->close();
	
	std::string readme_name(of + "/README");
	out = std::unique_ptr<std::ofstream>(new std::ofstream(readme_name.c_str(), std::ios::out));
	(*out) << "\n" << std::flush;
	out->close();
	
	std::string authors_name(of + "/AUTHORS");
	out = std::unique_ptr<std::ofstream>(new std::ofstream(authors_name.c_str(), std::ios::out));
	(*out) << "\n" << std::flush;
	out->close();
	
	std::string log_name(of + "/ChangeLog");
	out = std::unique_ptr<std::ofstream>(new std::ofstream(log_name.c_str(), std::ios::out));
	(*out) << "\n" << std::flush;
	out->close();
}

