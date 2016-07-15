#ifndef __DISASS_PPC_H__
#define __DISASS_PPC_H__

#include "config.h"
#include "disassembler.h"
#include "helpers.h"

// See some documentation in CPP file.

// Instruction class
#define PPC_DISA_OTHER      0x0000  // No additional information
#define PPC_DISA_64         0x0001  // 64-bit architecture only
#define PPC_DISA_INTEGER    0x0002  // Integer-type instruction
#define PPC_DISA_BRANCH     0x0004  // Branch instruction
#define PPC_DISA_LDST       0x0008  // Load-store instruction
#define PPC_DISA_STRING     0x0010  // Load-store string/multiple
#define PPC_DISA_FPU        0x0020  // Floating-point instruction
#define PPC_DISA_OEA        0x0040  // Supervisor level
#define PPC_DISA_OPTIONAL   0x0200  // Optional
#define PPC_DISA_BRIDGE     0x0400  // Optional 64-bit bridge
#define PPC_DISA_SPECIFIC   0x0800  // Implementation-specific
#define PPC_DISA_ILLEGAL    0x1000  // Illegal
#define PPC_DISA_SIMPLIFIED 0x8000  // Simplified mnemonic is used

struct PPCD_CB
{
	uint64_t	pc;                     // Program counter (input)
	uint32_t	instr;                  // Instruction (input)
	char		mnemonic[16];           // Instruction mnemonic.
	char		operands[64];           // Instruction operands.
	uint32_t	immed;                  // Immediate value (displacement for load/store, immediate operand for arithm./logic).
	int			r[4];                   // Index value for operand registers and immediates.
	uint64_t	targeta;                // Target address for branch instructions
	uint64_t	targetb;
	address		call;					//a literal address called as a function
	int			trace_call;				//a function call address that must be traced
	int     	iclass;                 // One or combination of PPC_DISA_* flags.
};

void    PPCDisasm(PPCD_CB *disa);
char*   PPCDisasmSimple(uint64_t pc, uint32_t instr);

class disass_ppc : public disassembler
{
	public:
		disass_ppc(exe_loader *own);
		int get_instruction(instr* &get, address addr);
		std::string trace_value(std::string &val);
	private:
		int ppc_type;	//32, 64, gekko, broadway
		static const char * t_cond[32];
		static const char *regname[];
		static char crname[];
		static char fregname[];
		static const char *b_opt[4];
		static const char * b_cond[8];
		static const char * b_ctr[16];
		static int bigendian;
		static PPCD_CB *o;
		
		void ill();
		char * simm(int val, int hex, int s);
		void put(const char * mnem, uint32_t mask, uint32_t chkval, int iclass);
		void trap(int L, int imm);
		void integer(const char *mnem, char form, int dab, int hex, int s, int crfD, int L, int imm);
		void cmp(const char *l, const char *i);
		void addi(const char *suffix);
		char *place_target(char *ptr, int comma);
		void bcx(int Disp, int L);
		void bx(void);
		void mcrf(void);
		void crop(const char *name, const char *simp, int ddd, int daa);
		void rlw(const char *name, int rb, int ins);
		void rld(char *name, int rb, int mtype);
		void ldst(const char *name, int x/*indexed*/, int load, int L, int string, int fload);
		void cache(const char *name, int flag);
		void movesr(const char *name, int from, int L, int xform);
		void mtcrf(void);
		void mcrxr(void);
		const char *spr_name(int n);
		const char *tbr_name(int n);
		void movespr(int from);
		void movetbr(void);
		void srawi(void);
		void sradi(void);
		void lsswi(const char *name);
		void fpu(const char *name, uint32_t mask, int type, int flag);
		void fcmp(const char *name);
		void mtfsf(void);
		void mtfsb(const char *name);
		void mcrfs(void);
		void mtfsfi(void);
		void ps_cmpx(int n);
		char *ps_ldst_offs(unsigned long val);
		void ps_ldst(char *fix);
		void ps_ldstx(char *fix);
		void ps_dacb(char *fix);
		void ps_dac(char *fix);
		void ps_dab(char *fix, int unmask);
		void ps_db(char *fix, int aonly);

		void PPCDisasm(PPCD_CB *discb);
		char *PPCDisasmSimple(uint64_t pc, uint32_t instr);
		uint64_t make_mask(int mb, int me, int numbits);
};

#endif
