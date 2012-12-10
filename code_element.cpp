#include "code_element.h"

code_element::code_element()
{
	ins = 0;
	s = NULL;
	a = NULL;
	b = NULL;
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
	return ins;
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
	return b;
}

void code_element::fprint(FILE *dest, int depth)
{
	printf("Dummy print\n");
}

void code_element::ss(void* ss)
{
	s = ss;
}

void *code_element::gets()
{
	return s;
}

void code_element::sa(code_element *aa)
{
	a = aa;
}

void code_element::sb(code_element *bb)
{
	b = bb;
}

void code_element::setbnum(int n)
{
	block_num = n;
}

int code_element::getbnum()
{
	return block_num;
}
