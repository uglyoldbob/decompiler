#include "disass_x86.h"

#include "config.h"
#include "exceptions.h"
#include "exe_loader.h"
#include <iostream>

disass_x86::disass_x86(exe_loader *own) :
	disassembler(own)
{
	ud_init(&u);
	ud_set_mode(&u, 16);
	ud_set_syntax(&u,UD_SYN_INTEL);
}

disass_x86::~disass_x86()
{
}

int disass_x86::get_instruction(instr* &get, address addr)
{
	unsigned char instr[15];
	owner->goto_address(addr);
	owner->read_memory((void*)instr, 15);
	ud_set_input_buffer(&u, instr, 15);
	ud_disassemble(&u);
	
	get = new class instr;
	get->addr = addr;
	get->ins = 0;
	get->is_cbranch = 0;
	get->destaddra = addr + ud_insn_len(&u);
	get->destaddrb = addr + ud_insn_len(&u);
	get->comment = std::string("\t//") + ud_insn_asm(&u);
	get->call = 0;
	std::string op(ud_lookup_mnemonic(ud_insn_mnemonic(&u)));
	
	if (op[0] == 'j')
	{
		const ud_operand_t *jmp_addr = ud_insn_opr(&u, 0);
		std::cout << "JUMP Operator is " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) 
				  << std::endl;
		switch (jmp_addr->type)
		{
		case UD_OP_JIMM:
			switch (jmp_addr->size)
			{
			case 8:
				std::cout << "Jump to address8 " << std::hex << (jmp_addr->lval.sbyte + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->destaddrb = (jmp_addr->lval.sbyte + addr + ud_insn_len(&u));
				break;
			case 16:
				std::cout << "Jump to address16 " << std::hex << (jmp_addr->lval.sword + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->destaddrb = (jmp_addr->lval.sword + addr + ud_insn_len(&u));
				break;
			case 32:
				std::cout << "Jump to address32 " << std::hex << (jmp_addr->lval.sdword + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->destaddrb = (jmp_addr->lval.sdword + addr + ud_insn_len(&u));
				break;
			case 64:
				std::cout << "Jump to address64 " << std::hex << (jmp_addr->lval.sqword + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->destaddrb = (jmp_addr->lval.sqword + addr + ud_insn_len(&u));
				break;
			default:
				std::cout << "Unknown jump address: " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) << std::endl;
				break;
			}
			break;
		default:
			std::cout << "Unknown jump address: " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) << std::endl;
			break;
		}
		if (op == "jmp")
		{
			get->destaddra = get->destaddrb;
			get->is_cbranch = 0;
		}
		else
		{
			get->destaddra = addr + ud_insn_len(&u);
			get->is_cbranch = 1;
		}
		std::cout << std::hex << addr << std::dec << ": " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) 
				  << " :::: " << ud_insn_asm(&u) << " [" << ud_insn_hex(&u) << "] " 
				  << ud_insn_len(&u) << std::endl;
	}
	else if ( (op == "loop") || (op == "loope") || (op == "loopz") || 
				(op == "loopne") || (op == "loopnz") )
	{
		std::cout << "LOOP Operator is " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) << std::endl;
	}
	else if (op == "call")
	{
	}
	else if (op == "ret")
	{
	}
	else
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
		std::cout << std::hex << addr << std::dec << ": " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) 
				  << " :::: " << ud_insn_asm(&u) << " [" << ud_insn_hex(&u) << "] " 
				  << ud_insn_len(&u) << std::endl;
	}
	//throw invalid_instruction(addr);
	
	return ud_insn_len(&u);
}

std::string disass_x86::trace_value(std::string &val)
{
	return "";
}
