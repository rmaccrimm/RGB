#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "definitions.h"

class Processor;

namespace op
{
    /*  imm - second argument taken from byte in memory following instruction
        mem - address of second argument stored in 16 bit register  
    */
    
    void NOP();
    void INVALID();
    void STOP();

    void LD(u8 &dest, u8 &src);
    void LD(reg16 &dest, reg16 &src);
    void LD_imm(Processor *proc, u8 &reg);
    void LD_imm(Processor *proc, reg16 &reg);
    void LD_mem(Processor *proc, u8 &dest, reg16 &src);
    void LD_mem(Processor *proc, reg16 &dest, u8 &src);

    void PUSH(Processor *proc, u8 &reg);
    void PUSH(Processor *proc, reg16 &reg);
    void POP(Processor *proc, u8 &reg);
    void POP(Processor *proc, reg16 &reg);

    void ADD(Processor *proc, u8 &dest, u8 &src);
    void ADD(Processor *proc, reg16 &dest, reg16 &src);
    void ADD_imm(Processor *proc, u8 &reg);
    void ADD_imm(Processor *proc, reg16 &reg);
    void ADD_mem(Processor *proc, u8 &dest, reg16 &src);
    void ADC(Processor *proc, u8 &dest, u8 &src);
    void ADC_imm(Processor *proc, u8 &dest);
    void ADC_mem(Processor *proc, u8 &dest, reg16 &src);

    void SUB(Processor *proc, u8 &dest, u8 &src);
    void SUB_imm(Processor *proc, u8 &reg);
    void SUB_mem(Processor *proc, u8 &dest, reg16 &src);
    void SBC(Processor *proc, u8 &dest, u8 &src);
    void SBC_imm(Processor *proc, u8 &dest);
    void SBC_mem(Processor *proc, u8 &dest, reg16 &src);

    void INC(Processor *proc, u8 &reg);
    void INC(reg16 &reg);
    void INC_mem(Processor *proc, reg16 &reg); 
    void DEC(Processor *proc, u8 &reg);
    void DEC(Processor *proc, reg16 &reg);
    void DEC_mem(Processor *proc, reg16 &reg);

    void AND(Processor *proc, u8 &dest, u8 &src);
    void AND_imm(Processor *proc, u8 &reg);
    void AND_mem(Processor *proc, u8 &dest, reg16 &src);
    void OR(Processor *proc, u8 &dest, u8 &src);
    void OR_imm(Processor *proc, u8 &reg);
    void OR_mem(Processor *proc, u8 &dest, reg16 &src);
    void XOR(Processor *proc, u8 &dest, u8 &src);
    void XOR_imm(Processor *proc, u8 &reg);
    void XOR_mem(Processor *proc, u8 &dest, reg16 &src);
    void CP(Processor *proc, u8 &dest, u8 &src);
    void CP_imm(Processor *proc, u8 &reg);
    void CP_mem(Processor *proc, u8 &dest, reg16 &src);

    void SWAP(Processor *proc, u8 &reg);
    void SWAP_mem(Processor *proc, reg16 &reg);

    void RL(Processor *proc, u8 &reg);
    void RL(Processor *proc, reg16 &reg);
    void RLC(Processor *proc, u8 &reg);
    void RLC(Processor *proc, reg16 &reg);
    void RR(Processor *proc, u8 &reg);
    void RR(Processor *proc, reg16 &reg);
    void RRC(Processor *proc, u8 &reg);
    void RRC(Processor *proc, reg16 &reg);

    void SLA(Processor *proc, u8 &reg);
    void SLA(Processor *proc, reg16 &reg);
    void SRA(Processor *proc, u8 &reg);
    void SRA(Processor *proc, reg16 &reg);
    void SRL(Processor *proc, u8 &reg);
    void SRL(Processor *proc, reg16 &reg);

    void BIT(Processor *proc, u8 &reg, u8 bit);
    void BIT(Processor *proc, reg16 &reg, u8 bit);
    void SET(u8 &reg, u8 bit);
    void SET(Processor *proc, reg16 &reg, u8 bit);
    void RES(u8 &reg, u8 bit);
    void RES(Processor *proc, reg16 &reg, u8 bit);

    void JP(Processor *proc, bool cond);
    void JP(Processor *proc, reg16 &reg);
    void JR(Processor *proc, bool cond);
    void CALL(Processor *proc, bool cond);
    void RET(Processor *proc, bool cond);

    void RST(Processor *proc, u8 addr);
    void DAA(Processor *proc);
    void CPL(Processor *proc);
    void SCF(Processor *proc);
    void CCF(Processor *proc);
}

#endif
