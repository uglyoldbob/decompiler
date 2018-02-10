#include "config.h"
#include "src/var/related_code_maker.h"

int main(int argc, char *argv[])
{
	int retval = 0;

	int failures = 0;
	related_code_maker rcm;
	
	for (int i = 2; i < 7; i++)
	{
		if (failures < 10)
		{
			rcm.set_num_elements(i);
			while ((rcm.next()) && (failures < 10))
			{
				rcm.update();
				std::cout << "STARTING with (" << i << ")\n";
				rcm.get_rc()->print_graph(std::cout);
				rcm.simplify();
				if (!rcm.simplified())
				{
					std::cout << "Failed to simplify " << i << "\n";
					//failures++;
				}
				else
				{
					std::cout << "Succeeded simplifying " << i << "\n";
				}
				rcm.get_rc()->print_graph(std::cout);
				std::cout << "CODE OUTPUT\n";
				rcm.get_rc()->fprint(std::cout, 1);
				std::cout << std::endl << std::endl << std::endl;
				retval = 1;
			}
		}
	}
	std::cout << std::endl;
	
	return retval;
}
