#include "config.h"
#include "src/var/related_code_maker.h"

int main(int argc, char *argv[])
{
	int retval = 0;

	related_code_maker rcm;
	
	for (int i = 2; i < 7; i++)
	{
		rcm.set_num_elements(i);
		while (rcm.next())
		{
			rcm.update();
			rcm.get_rc()->print_graph(std::cout);
			rcm.simplify();
			if (!rcm.simplified())
			{
				std::cout << "Failed to simplify " << i << "\n";
			}
			else
			{
				std::cout << "Worked to simplify " << i << "\n";
			}
			rcm.get_rc()->print_graph(std::cout);
			rcm.get_rc()->fprint(std::cout, 1);
			std::cout << std::endl;
			retval = 1;
		}
	}
	std::cout << std::endl;
	
	return retval;
}
