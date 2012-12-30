#include "code_element.h"

code_element::code_element()
{
	s = 0;
	a = 0;
	b = 0;
	depth = 0;
}

code_element::~code_element()
{
}

int code_element::is_cbranch()
{	//does this element have a conditional branch at the end
	if ((a != 0) && (b != 0))
		return 1;
	else
		return 0;
}

int code_element::gins()	//get ins
{
	return inputs.size();
}

code_element *code_element::ga()
{
	if (a == 0)
		return b;
	else
		return a;
}

code_element *code_element::gb()
{
	if (a == 0)
		return a;
	else
		return b;
}

void code_element::replace_references(code_element *old, code_element *nw)
{
	if (a == old)
		a = nw;
	if (b == old)
		b = nw;
}

void code_element::copy_inputs(code_element *src)
{
	inputs = src->inputs;
}

void code_element::fprint(std::ostream &dest, int depth)
{
	dest << "Dummy print\n";
}

address code_element::gets()
{
	return s;
}

void code_element::remove_input(code_element *me)	//decrease ins
{
	for (unsigned int i = 0; i < inputs.size(); ++i)
	{
		if (inputs[i] == me)
		{
			inputs.erase(inputs.begin() + i);
			break;
		}
	}
	//cout << "Reduce ins of " << std::hex << s << std::dec << " (" << ins << ") by " << by << "\n";
	//ins -= by;
}
