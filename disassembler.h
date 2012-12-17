#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

class exe_loader;

class disassembler
{
	public:
		disassembler(exe_loader *own);
		virtual ~disassembler();
	private:
		exe_loader *owner;
};

#endif