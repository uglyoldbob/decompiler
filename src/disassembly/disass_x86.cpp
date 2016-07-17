#include "disass_x86.h"

#include "config.h"
#include "exceptions.h"
#include "executable/exe_loader.h"
#include "operators/operators_all.h"
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

std::string get_type(ud_type_t me)
{
	switch(me)
	{
	case UD_R_RIP: 	return "ip";
	case UD_R_AL: return "al";
	case UD_R_AH: return "ah";
	case UD_R_BL: return "bl";
	case UD_R_BH: return "bh";
	case UD_R_CL: return "cl";
	case UD_R_CH: return "ch";
	case UD_R_DL: return "dl";
	case UD_R_DH: return "dh";
	case UD_R_SPL: return "spl";
	case UD_R_BPL: return "bpl";
	case UD_R_SIL: return "sil";
	case UD_R_DIL: return "dil";
	case UD_R_R8B: case UD_R_R9B: case UD_R_R10B:
	case UD_R_R11B: case UD_R_R12B: case UD_R_R13B:
	case UD_R_R14B: case UD_R_R15B:
		throw "unknown 8-bit register";
		return "unknown 8-bit";
		break;
	case UD_R_AX: return "ax";
	case UD_R_CX: return "cx";
	case UD_R_DX: return "dx";
	case UD_R_BX: return "bx";
	case UD_R_SP: return "sp";
	case UD_R_BP: return "bp";
	case UD_R_SI: return "si";
	case UD_R_DI: return "di";
	case UD_R_R8W: 	case UD_R_R9W: case UD_R_R10W:
	case UD_R_R11W: case UD_R_R12W: case UD_R_R13W:
	case UD_R_R14W: case UD_R_R15W:
		throw "unknown 16-bit register";
		return "unknown 8-bit";
	case UD_R_ES: return "es";
	case UD_R_CS: return "cs";
	case UD_R_SS: return "ss";
	case UD_R_DS: return "ds";
	case UD_R_FS: return "fs";
	case UD_R_GS: return "gs";
/* 
  32 bit GPRs
  UD_R_EAX, UD_R_ECX, UD_R_EDX, UD_R_EBX,
  UD_R_ESP, UD_R_EBP, UD_R_ESI, UD_R_EDI,
  UD_R_R8D, UD_R_R9D, UD_R_R10D,  UD_R_R11D,
  UD_R_R12D,  UD_R_R13D,  UD_R_R14D,  UD_R_R15D,
  
  64 bit GPRs 
  UD_R_RAX, UD_R_RCX, UD_R_RDX, UD_R_RBX,
  UD_R_RSP, UD_R_RBP, UD_R_RSI, UD_R_RDI,
  UD_R_R8,  UD_R_R9,  UD_R_R10, UD_R_R11,
  UD_R_R12, UD_R_R13, UD_R_R14, UD_R_R15,
*/  

	default: throw "unknown register"; return "unknown";	break;
	}
	return "broken";
}

variable *disass_x86::interpret_operand(const ud_operand_t *m)
{
	variable *ret = 0;

	switch (m->type)
	{
	case UD_OP_MEM:
		{
		variable *seg = 0;
		variable *base = 0;
		variable *index = 0;
		variable *scale = 0;
		variable *offset = 0;
		//seg:base + index * scale + offset
		if (u.pfx_seg != UD_NONE)
		{
			seg = new variable(get_type((ud_type_t)u.pfx_seg), -1);
		}
		if (m->base != UD_NONE)
		{
			base = new variable(get_type(m->base), -1);
		}
		if (m->index != UD_NONE)
		{
			index = new variable(get_type(m->index), -1);
		}
		if (m->scale != 0)
		{
			std::stringstream s;
			s << std::dec;
			s << m->scale << std::flush;
			scale = new variable(s.str(), -1);
		}
		switch (m->offset)
		{
		case 8:
			{
			std::stringstream s;
			s << std::dec;
			s << (int16_t)m->lval.sbyte << std::flush;
			offset = new variable(s.str(), 1);
			if ((m->index != UD_NONE) || (m->base != UD_NONE))
			{
				if (m->lval.sbyte > 0)
				{
				//	std::cout << " + ";
				//	std::cout << (int16_t)mov_from->lval.sbyte;
				}
				else
				{
				//	std::cout << " - ";
				//	std::cout << -(int16_t)mov_from->lval.sbyte;
				}
			}
			else
			{
			//	std::cout << (int16_t)mov_from->lval.sbyte;
			}
			}
			break;
		case 16:
			{
			std::stringstream s;
			s << std::dec;
			s << m->lval.sword << std::flush;
			offset = new variable(s.str(), 2);
			if ((m->index != UD_NONE) || (m->base != UD_NONE))
			{
				if (m->lval.sword > 0)
				{
				//	std::cout << " + ";
				//	std::cout << (int16_t)mov_from->lval.sword;
				}
				else
				{
				//	std::cout << " - ";
				//	std::cout << -(int16_t)mov_from->lval.sword;
				}
			}
			else
			{
			//	std::cout << (int16_t)mov_from->lval.sword;
			}
			}
			break;
		case 32:
			{
			std::stringstream s;
			s << std::dec;
			s << m->lval.sdword << std::flush;
			offset = new variable(s.str(), 4);
			if ((m->index != UD_NONE) || (m->base != UD_NONE))
			{
				if (m->lval.sdword > 0)
				{
				//	std::cout << " + ";
				//	std::cout << (int16_t)mov_from->lval.sdword;
				}
				else
				{
				//	std::cout << " - ";
				//	std::cout << -(int16_t)mov_from->lval.sdword;
				}
			}
			else
			{
			//	std::cout << (int16_t)mov_from->lval.sdword;
			}
			}
			break;
		case 64:
			{
			std::stringstream s;
			s << std::dec;
			s << m->lval.sqword << std::flush;
			offset = new variable(s.str(), 8);
			if ((m->index != UD_NONE) || (m->base != UD_NONE))
			{
				if (m->lval.sqword > 0)
				{
				//	std::cout << " + ";
				//	std::cout << (int16_t)mov_from->lval.sqword;
				}
				else
				{
				//	std::cout << " - ";
				//	std::cout << -(int16_t)mov_from->lval.sqword;
				}
			}
			else
			{
			//	std::cout << (int16_t)mov_from->lval.sqword;
			}
			}
			break;
		default:
			break;
		}
		if ( (seg != 0) || (base != 0) || (index != 0) || (scale != 0) || (offset != 0) )
		{
			if ( (seg != 0) || (base != 0) )
			{
				if (base == 0)
				{
					base = offset;
					offset = 0;
				}
				ret = new oper_segbase(seg, base);
			}
			if (index != 0)
			{
				if (ret == 0)
				{
					ret = index;
				}
				else
				{
					ret = new oper_add(ret, index);
				}
			}
			if (scale != 0)
			{
				if (ret == 0)
				{
					ret = scale;
				}
				else
				{
					ret = new oper_add(ret, scale);
				}
			}
			if (offset != 0)
			{
				if (ret == 0)
				{
					ret = offset;
				}
				else
				{
					ret = new oper_add(ret, offset);
				}
			}
			if (ret != 0)
			{
				ret = new oper_dereference(ret);
			}
		}
		}
		break;
	case UD_OP_PTR:
		break;
	case UD_OP_REG:
		ret = new variable(get_type(m->base), -1);
		break;
	case UD_OP_IMM:
	case UD_OP_CONST:
		{
		std::stringstream s;
		s << std::dec;
		switch (m->size)
		{
		case 16:
			s << ((int16_t)m->lval.sword) << std::flush;
			break;
		case 32:
			s << ((int32_t)m->lval.sdword) << std::flush;
			break;
		case 64:
			s << ((int64_t)m->lval.sqword) << std::flush;
			break;
		default:
			s << ((int16_t)m->lval.sbyte) << std::flush;
			break;
		}
		ret = new variable(s.str(), m->size/8);
		}
		break;
	default:
		break;
	}
	
	return ret;
}

int disass_x86::get_instruction(instr* &get, address addr)
{
	unsigned char instr[15];
	owner->goto_address(addr);
	owner->read_memory((void*)instr, 15);
	ud_set_input_buffer(&u, instr, 15);
	ud_set_pc(&u, addr);
	ud_disassemble(&u);
	
	get = new struct instr;
	get->addr = addr;
	get->is_cbranch = 0;
	get->is_branch = false;
	get->is_ret = false;
	get->valid = false;
	get->destaddra = addr + ud_insn_len(&u);
	get->destaddrb = addr + ud_insn_len(&u);
	get->comment = std::string("//") + ud_insn_asm(&u);
	get->call = 0;
	get->len = ud_insn_len(&u);
	get->trace_call = 0;
	get->trace_jump = 0;
	std::string op(ud_lookup_mnemonic(ud_insn_mnemonic(&u)));
	
	if ((op[0] == 'j') || (op == "loop") || (op == "loope") || 
		(op == "loopz") || 	(op == "loopne") || (op == "loopnz") )
	{
		const ud_operand_t *jmp_addr = ud_insn_opr(&u, 0);
		get->is_branch = true;
		switch (jmp_addr->type)
		{
		case UD_OP_JIMM:
			switch (jmp_addr->size)
			{
			case 8:
				get->valid = true;
				get->destaddrb = (jmp_addr->lval.sbyte + addr + ud_insn_len(&u));
				break;
			case 16:
				get->valid = true;
				get->destaddrb = (jmp_addr->lval.sword + addr + ud_insn_len(&u));
				break;
			case 32:
				get->valid = true;
				get->destaddrb = (jmp_addr->lval.sdword + addr + ud_insn_len(&u));
				break;
			case 64:
				get->valid = true;
				get->destaddrb = (jmp_addr->lval.sqword + addr + ud_insn_len(&u));
				break;
			default:
				get->trace_jump = new variable("unknown size??", -1);
				break;
			}
			break;
		case UD_OP_MEM:
			get->trace_jump = new variable("memory ref??", -1);
			break;
		case UD_OP_PTR:
			get->destaddrb = (jmp_addr->lval.ptr.seg*0x10 + jmp_addr->lval.ptr.off);
			break;
		case UD_OP_IMM:
			std::cout << "Unknown jump immediate" << std::endl;
			get->trace_jump = new variable("immediate??", -1);
			break;
		case UD_OP_CONST:
			std::cout << "Unknown jump const" << std::endl;
			get->trace_jump = new variable("const??", -1);
			break;
		case UD_OP_REG:
			std::cout << "Unknown jump register" << std::endl;
			get->trace_jump = new variable(get_type(jmp_addr->base), -1);
			break;
		default:
			std::cout << "Unknown jump address: " << ud_lookup_mnemonic(ud_insn_mnemonic(&u)) << std::endl;
			get->trace_jump = new variable("not an immediate??", -1);
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
		switch (jmp_addr->type)
		{
		case UD_OP_JIMM:
			switch (jmp_addr->size)
			{
			case 8:
				get->call = (jmp_addr->lval.sbyte + addr + ud_insn_len(&u));
				break;
			case 16:
				get->call = (jmp_addr->lval.sword + addr + ud_insn_len(&u));
				break;
			case 32:
				get->call = (jmp_addr->lval.sdword + addr + ud_insn_len(&u));
				break;
			case 64:
				get->call = (jmp_addr->lval.sqword + addr + ud_insn_len(&u));
				break;
			default:
				get->trace_call = new variable("??", -1);
				break;
			}
			break;
		default:
			get->trace_call = new variable("??", -1);
			break;
		}
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
	}
	else if (op == "ret")
	{
		get->destaddra = 0;
		get->destaddrb = 0;
		get->is_ret = true;
	}
	else if (op == "mov")
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
		variable *to = interpret_operand(ud_insn_opr(&u, 0));
		variable *from = interpret_operand(ud_insn_opr(&u, 1));
		
		if ((from != 0) && (to != 0))
		{
			get->valid = true;
			get->statements.push_back(new oper_assignment(to, from));
		}
		else
		{
			if (from != 0)
				delete from;
			if (to != 0)
				delete to;
		}
	}
	else if (op == "inc")
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
		variable *to = interpret_operand(ud_insn_opr(&u, 0));
	
		if (to != 0)
		{
			get->valid = true;
			get->statements.push_back(new oper_assignment(to, new oper_add(to, new variable("1", -1))));
		}
	}
	else if (op == "dec")
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
		variable *to = interpret_operand(ud_insn_opr(&u, 0));
	
		if (to != 0)
		{
			get->valid = true;
			get->statements.push_back(new oper_assignment(to, new oper_sub(to, new variable("1", -1))));
		}
	}
	else if (op == "add")
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
		variable *to = interpret_operand(ud_insn_opr(&u, 0));
		variable *from = interpret_operand(ud_insn_opr(&u, 1));
		
		if ((from != 0) && (to != 0))
		{
			get->valid = true;
			get->statements.push_back(new oper_assignment(to, new oper_add(to, from)));
		}
		else
		{
			if (from != 0)
				delete from;
			if (to != 0)
				delete to;
		}
	}
	else if (op == "sub")
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
		variable *to = interpret_operand(ud_insn_opr(&u, 0));
		variable *from = interpret_operand(ud_insn_opr(&u, 1));
		
		if ((from != 0) && (to != 0))
		{
			get->valid = true;
			get->statements.push_back(new oper_assignment(to, new oper_sub(to, from)));
		}
		else
		{
			if (from != 0)
				delete from;
			if (to != 0)
				delete to;
		}
	}
	else if (op == "and")
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
		variable *to = interpret_operand(ud_insn_opr(&u, 0));
		variable *from = interpret_operand(ud_insn_opr(&u, 1));
		
		if ((from != 0) && (to != 0))
		{
			get->valid = true;
			get->statements.push_back(new oper_assignment(to, new oper_bitwise_and(to, from)));
		}
		else
		{
			if (from != 0)
				delete from;
			if (to != 0)
				delete to;
		}
	}
	else if (op == "shr")
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
		variable *to = interpret_operand(ud_insn_opr(&u, 0));
		variable *from = interpret_operand(ud_insn_opr(&u, 1));
		
		if ((from != 0) && (to != 0))
		{
			get->valid = true;
			get->statements.push_back(new oper_assignment(to, new oper_right_shift(to, from)));
		}
		else
		{
			if (from != 0)
				delete from;
			if (to != 0)
				delete to;
		}
	}
	else if (op == "shl")
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
		variable *to = interpret_operand(ud_insn_opr(&u, 0));
		variable *from = interpret_operand(ud_insn_opr(&u, 1));
		
		if ((from != 0) && (to != 0))
		{
			get->valid = true;
			get->statements.push_back(new oper_assignment(to, new oper_left_shift(to, from)));
		}
		else
		{
			if (from != 0)
				delete from;
			if (to != 0)
				delete to;
		}
	}

	else
	{
		get->destaddra = addr + ud_insn_len(&u);
		get->destaddrb = 0;
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
	return ud_insn_len(&u);
}

