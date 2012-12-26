#include "ce_block.h"
#include "code_element.h"

ce_block::ce_block()
{
	num_lines = 0;
	line = NULL;
}

ce_block::~ce_block()
{
	delete [] line;
}

void ce_block::fprint(std::ostream &dest, int depth)
{
	int i;
	begin_line(dest, depth);
	dest << "/------";
	if (depth == 0)
		dest << std::hex << s << std::dec << " (" << line[0]->ins << "input)";
	dest << "\n";	
	int k;
	for (k = 0; k < num_lines; k++)
	{
		begin_line(dest, depth);
		dest << "|"
			 << std::hex << line[k]->addr << std::dec << " "
			 << line[k]->opcode << " " << line[k]->options << " " << line[k]->comment << "\n";
	}
	begin_line(dest, depth);
	dest << "\\------ ";
	if (depth == 0)
	{
		if (a != 0)
			dest << std::hex << a->gets() << std::dec << " ";
		if (b != 0)
			dest << std::hex << b->gets() << std::dec << " ";
	}
	dest << "\n";
}

void ce_block::setnline(int num)
{
	if (num_lines == 0)
	{
		num_lines = num;
		line = new line_info*[num_lines];
	}
}

int ce_block::getnline()
{
	return num_lines;
}

void ce_block::setline(line_info *a)
{
	int i;
	for (i = 0; i < num_lines; i++)
	{
		line[i] = &a[i];
	}
	ins = line[0]->ins;
}

line_info *ce_block::getline(int num)
{	//-1 means get last line
	if (num_lines == 0)
	{
		return NULL;
	}
	else if (num < -1)
	{
		return NULL;
	}
	else if (num == -1)
	{
		return line[num_lines-1];
	}
	else if (num < num_lines)
	{
		return line[num];
	}
	return 0;
}
