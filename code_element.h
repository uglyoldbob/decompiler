#ifndef __CODE_ELEMENT_H__
#define __CODE_ELEMENT_H__

#include <stdio.h>

void begin_line(FILE *b, int a);

struct line_info
{
	void *addr;
	char opcode[10];
	char options[300];
	char comment[300];
	int ins;	//increase when control is branched to here
	int is_cbranch;	//set when it is a conditional branch statement
	void *destaddra;	//stores the dest of the conditional branch statement
	void *destaddrb;	//stores the other dest
	int line_num;	//makes it easier for a code block to find the first line of the next block
};

struct code_block
{	//a code block starts in multiple cases
	//the first line of code
	//after any branch instruction
	void *s;	//the starting address
	struct line_info **in;
	int num_lines;
	struct code_block *a;	//0 means last block
	struct code_block *b;	//0 means there is not one
};

class code_element
{
	public:
		code_element();
		virtual ~code_element();
		
		void ss(void *ss);	//sets s
		void *gets();		//gets s
		void sa(code_element *aa);	//sets a
		code_element *ga();
		void sb(code_element *bb);	//sets b
		code_element *gb();
		virtual void fprint(FILE *dest, int depth);
		int gins();	//get ins
		void dins(int by);	//decrease ins
		int is_cbranch();	//does this element have a conditional branch at the end
	protected:
		int ins;	//number of elements that point to this one
		void *s;	//the starting address
		code_element *a;	//next element
		code_element *b;	//other next element (if a decision has to be made)
};

#endif