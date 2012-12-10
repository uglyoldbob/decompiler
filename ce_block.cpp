#include "ce_block.h"

ce_block::ce_block()
{
	num_lines = 0;
	line = NULL;
}

ce_block::~ce_block()
{
	if (line != NULL)
		delete [] line;
}

void ce_block::fprint(FILE *dest, int depth)
{
	int i;
	for (i = 0; i < depth; i++) { fprintf(dest, "\t");}
	fprintf(dest, "/------");
	if (depth == 0)
		fprintf(dest, "%x (%d input)", s, line[0]->ins);
	fprintf(dest, "\n");	
	int k;
	for (k = 0; k < num_lines; k++)
	{
		for (i = 0; i < depth; i++) { fprintf(dest, "\t");}
		fprintf(dest, "|%x %s %s %s\n", line[k]->addr, line[k]->opcode, line[k]->options, line[k]->comment);
	}
	for (i = 0; i < depth; i++) { fprintf(dest, "\t");}
	fprintf(dest, "\\------ ");
	if (depth == 0)
	{
		if (a != 0)
			fprintf(dest, "%x ", a->gets());
		if (b != 0)
			fprintf(dest, "%x ", b->gets());
	}
	fprintf(dest, "\n");
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
