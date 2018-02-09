#include "config.h"
#include "project/project.h"
#include "code_elements/related_code.h"
#include <iostream>

int main(int argc, char *argv[])
{
	int retval = 0;

	int failures = 0;
	project *test_proj;
	test_proj = new project(0);
	related_code::list_code_element_makers(std::cout);
	delete test_proj;
	return retval;
}
