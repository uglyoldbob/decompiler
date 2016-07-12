#include "code_element.h"

#include "exceptions.h"
#include "helpers.h"

code_element::code_element(address addr)
	: s(addr)
{
	a = 0;
	b = 0;
}

code_element::code_element()
	: s(0)
{
	a = 0;
	b = 0;
}

code_element::~code_element()
{
	a = 0;
	b = 0;
}

//Returns the addresses this element might flow to
std::vector<address> code_element::get_nexts()
{
	std::vector<address> ret;

	if (a != 0)
		ret.push_back(a->s);
	if (b != 0)
		ret.push_back(b->s);
	return ret;
}

void code_element::replace_references(code_element *old, code_element *nw)
{
	if (a == old)
		a = nw;
	if (b == old)
		b = nw;
}

void code_element::print_graph(std::ostream &dest)
{
	if (a != 0)
		dest << "\tX" << s// << "_" << pieces[i] 
		 << " -> X" << a->s // << "_" << pieces[i]->ga()  
		 << " [ label=a ];\n";
	if (b != 0)
		dest << "\tX" << s// << "_" << pieces[i] 
		 << " -> X" << b->s// << "_" << pieces[i]->gb() 
		 << " [ label=b ];\n";
}

void code_element::fprint(std::ostream &dest, int depth)
{
}

address code_element::gets()
{
	return s;
}

