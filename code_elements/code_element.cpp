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

bool code_element::is_branch()
{
	return ((a != 0) && (b != 0)); 
}

bool code_element::jumps_to(code_element *m)
{
	return (!is_branch() && 
		( (a == m) || (b == m) ) 
		);		
}

bool code_element::branches_to(code_element *m)
{
	return (is_branch() && 
		( (a == m) || (b == m) ) 
		);
}

code_element *code_element::other_branch(code_element *m)
{
	code_element *ret = 0;
	if (is_branch())
	{
		if (a != m)
			ret = a;
		else if (b != m)
			ret = b;
	}
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

address code_element::gets()
{
	return s;
}

