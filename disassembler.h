#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

struct instr
{
	void *addr;
	char *opcode;
	char *options;
	char *comment;
	unsigned char length;
	int ins;	//increase when control is branched to here
	int is_cbranch;	//set when it is a conditional branch statement
	void *destaddra;	//stores the dest of the conditional branch statement
	void *destaddrb;	//stores the other dest
	int line_num;	//makes it easier for a code block to find the first line of the next block
};

class exe_loader;

class disassembler
{
	public:
		disassembler(exe_loader *own);
		virtual ~disassembler();
		
		virtual instr *get_instruction(void *addr) = 0;
	private:
		exe_loader *owner;
};

#endif