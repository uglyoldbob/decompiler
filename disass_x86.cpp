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
	ud_set_pc(&u, addr);
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
	
	if ((op[0] == 'j') || (op == "loop") || (op == "loope") || 
		(op == "loopz") || 	(op == "loopne") || (op == "loopnz") )
	{
		const ud_operand_t *jmp_addr = ud_insn_opr(&u, 0);
	//	std::cout << "JUMP Operator is " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) 
	//			  << std::endl;
		switch (jmp_addr->type)
		{
		case UD_OP_JIMM:
			switch (jmp_addr->size)
			{
			case 8:
	//			std::cout << "Jump to address8 " << std::hex << (jmp_addr->lval.sbyte + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->destaddrb = (jmp_addr->lval.sbyte + addr + ud_insn_len(&u));
				break;
			case 16:
	//			std::cout << "Jump to address16 " << std::hex << (jmp_addr->lval.sword + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->destaddrb = (jmp_addr->lval.sword + addr + ud_insn_len(&u));
				break;
			case 32:
	//			std::cout << "Jump to address32 " << std::hex << (jmp_addr->lval.sdword + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->destaddrb = (jmp_addr->lval.sdword + addr + ud_insn_len(&u));
				break;
			case 64:
	//			std::cout << "Jump to address64 " << std::hex << (jmp_addr->lval.sqword + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->destaddrb = (jmp_addr->lval.sqword + addr + ud_insn_len(&u));
				break;
			default:
	//			std::cout << "Unknown jump address: " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) << std::endl;
				get->trace_jump = "unknown size??";
				break;
			}
			break;
		case UD_OP_MEM:
			std::cout << "Unknown jump memory reference" << std::endl;
			get->trace_jump = "memory ref??";
			break;
		case UD_OP_PTR:
			get->destaddrb = (jmp_addr->lval.ptr.seg*0x10 + jmp_addr->lval.ptr.off);
			break;
		case UD_OP_IMM:
			std::cout << "Unknown jump immediate" << std::endl;
			get->trace_jump = "immediate??";
			break;
		case UD_OP_CONST:
			std::cout << "Unknown jump const" << std::endl;
			get->trace_jump = "const??";
			break;
		case UD_OP_REG:
			std::cout << "Unknown jump register" << std::endl;
			get->trace_jump = "register??";
			break;
		default:
			std::cout << "Unknown jump address: " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) << std::endl;
			get->trace_jump = "not an immediate??";
			break;
		}
		if (op == "jmp")
		{
			get->destaddra = get->destaddrb;
			get->destaddrb = 0;
			get->is_cbranch = 0;
		}
		else
		{
			get->destaddra = addr + ud_insn_len(&u);
			get->is_cbranch = 1;
		}
	/*	std::cout << std::hex << addr << ": " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) 
				  << " :::: " << ud_insn_asm(&u) << " [" << ud_insn_hex(&u) << "] " 
				  << ud_insn_len(&u) << " " << get->destaddra << " " << get->destaddrb << " " 
				  << get->is_cbranch 
				  << std::dec << std::endl;*/
	}
	else if (op == "invalid")
	{
		std::cout << std::hex << addr << ": " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) 
				  << " :::: " << ud_insn_asm(&u) << " [" << ud_insn_hex(&u) << "] " 
				  << ud_insn_len(&u) << " " << get->destaddra << " " << get->destaddrb << " " 
				  << get->is_cbranch 
				  << std::dec << std::endl;
		throw "Invalid opcode";
	}
	else if (op == "call")
	{
		const ud_operand_t *jmp_addr = ud_insn_opr(&u, 0);
	//	std::cout << "CALL Operator is " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) 
	//			  << std::endl;
		switch (jmp_addr->type)
		{
		case UD_OP_JIMM:
			switch (jmp_addr->size)
			{
			case 8:
	//			std::cout << "CALL to address8 " << std::hex << (jmp_addr->lval.sbyte + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->call = (jmp_addr->lval.sbyte + addr + ud_insn_len(&u));
				break;
			case 16:
	//			std::cout << "CALL to address16 " << std::hex << (jmp_addr->lval.sword + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->call = (jmp_addr->lval.sword + addr + ud_insn_len(&u));
				break;
			case 32:
	//			std::cout << "CALL to address32 " << std::hex << (jmp_addr->lval.sdword + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->call = (jmp_addr->lval.sdword + addr + ud_insn_len(&u));
				break;
			case 64:
	//			std::cout << "CALL to address64 " << std::hex << (jmp_addr->lval.sqword + addr + ud_insn_len(&u)) << std::dec << std::endl;
				get->call = (jmp_addr->lval.sqword + addr + ud_insn_len(&u));
				break;
			default:
	//			std::cout << "Unknown CALL address: " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) << std::endl;
				get->trace_call = "??";
				break;
			}
			break;
		default:
	//		std::cout << "Unknown CALL address: " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) << std::endl;
			get->trace_call = "??";
			break;
		}
	//	std::cout << std::hex << addr << std::dec << ": " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) 
	//			  << " :::: " << ud_insn_asm(&u) << " [" << ud_insn_hex(&u) << "] " 
	//			  << ud_insn_len(&u) << std::endl;
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
	}
	else if (op == "ret")
	{
		get->destaddra = 0;
		get->destaddrb = 0;
	}
	else
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
	//	std::cout << std::hex << addr << std::dec << ": " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) 
	//			  << " :::: " << ud_insn_asm(&u) << " [" << ud_insn_hex(&u) << "] " 
	//			  << ud_insn_len(&u) << std::endl;
	}
	
	if ( (get->destaddra != 0) &&
		 (get->destaddrb != 0) &&
		 (get->destaddra != get->destaddrb) )
	{
		get->is_cbranch = 1;
	}
	else
	{
		get->is_cbranch = 0;
	}
	//throw invalid_instruction(addr);
	
	return ud_insn_len(&u);
}

std::string disass_x86::trace_value(std::string &val)
{
	return "";
}
