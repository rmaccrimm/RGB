#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "Register8bit.h"
#include "Register16bit.h"

class Processor;

namespace op
{
    /*  imm - immediate, second argument taken from byte in memory following instruction
        mem - memory, address of second argument stored in 16 bit register  
    */

    typedef Register8bit r8;
    typedef Register16bit r16;

    void NOP() {}
    void INVALID();
    void HALT();
    void STOP();

    void LD(r8 &dest, r8 const &src);
    void LD(r16 &dest, r16 const &src);
    void LD_imm(Processor *proc, r8 &reg);
    void LD_imm(Processor *proc, r16 &reg);
    void LD_mem(Processor *proc, r8 &dest, r16 const &src);
    void LD_mem(Processor *proc, r16 const &dest, r8 const &src);

    void PUSH(Processor *proc, r8 const &reg);
    void PUSH(Processor *proc, r16 const &reg);
    void POP(Processor *proc, r8 &reg);
    void POP(Processor *proc, r16 &reg);

    void ADD(Processor *proc, r8 &dest, r8 const &src);
    void ADD(Processor *proc, r16 &dest, r16 const &src);
    void ADD_imm(Processor *proc, r8 &reg);
    void ADD_imm(Processor *proc, r16 &reg);
    void ADD_mem(Processor *proc, r8 &dest, r16 const &src);
    void ADC(Processor *proc, r8 &dest, r8 const &src);
    void ADC_imm(Processor *proc, r8 &dest);
    void ADC_mem(Processor *proc, r8 &dest, r16 const &src);

    void SUB(Processor *proc, r8 &dest, r8 const &src);
    void SUB_imm(Processor *proc, r8 &reg);
    void SUB_mem(Processor *proc, r8 &dest, r16 const &src);
    void SBC(Processor *proc, r8 &dest, r8 const &src);
    void SBC_imm(Processor *proc, r8 &dest);
    void SBC_mem(Processor *proc, r8 &dest, r16 const &src);

    void INC(Processor *proc, r8 &reg);
    void INC(Processor *proc, r16 &reg);
    void INC_mem(Processor *proc, r16 const &reg); 
    void DEC(Processor *proc, r8 &reg);
    void DEC(Processor *proc, r16 &reg);
    void DEC_mem(Processor *proc, r16 const &reg);

    void AND(Processor *proc, r8 &dest, r8 const &src);
    void AND_imm(Processor *proc, r8 &reg);
    void AND_mem(Processor *proc, r8 &dest, r16 const &src);
    void OR(Processor *proc, r8 &dest, r8 const &src);
    void OR_imm(Processor *proc, r8 &reg);
    void OR_mem(Processor *proc, r8 &dest, r16 const &src);
    void XOR(Processor *proc, r8 &dest, r8 const &src);
    void XOR_imm(Processor *proc, r8 &reg);
    void XOR_mem(Processor *proc, r8 &dest, r16 const &src);
    void CP(Processor *proc, r8 &dest, r8 const &src);
    void CP_imm(Processor *proc, r8 &reg);
    void CP_mem(Processor *proc, r8 &dest, r16 const &src);

    void SWAP(Processor *proc, r8 &reg);
    void SWAP(Processor *proc, r16 const &reg);

    void RL(Processor *proc, r8 &reg);
    void RR(Processor *proc, r8 &reg);
    void RLC(Processor *proc, r8 &reg);
    void RRC(Processor *proc, r8 &reg);
    void RL(Processor *proc, r16 const &reg);
    void RR(Processor *proc, r16 const &reg);
    void RLC(Processor *proc, r16 const &reg);
    void RRC(Processor *proc, r16 const &reg);

    void SLA(Processor *proc, r8 &reg);
    void SRA(Processor *proc, r8 &reg);
    void SRL(Processor *proc, r8 &reg);
    void SLA(Processor *proc, r16 const &reg);
    void SRA(Processor *proc, r16 const &reg);
    void SRL(Processor *proc, r16 const &reg);

    void BIT(Processor *proc, r8 &reg, u8 bit);
    void BIT(Processor *proc, r16 const &reg, u8 bit);
    void SET(Processor *proc, r8 &reg, u8 bit);
    void SET(Processor *proc, r16 const &reg, u8 bit);
    void RES(Processor *proc, r8 &reg, u8 bit);
    void RES(Processor *proc, r16 const &reg, u8 bit);

    void JP_cond(Processor *proc, bool cond);
    void JR_cond(Processor *proc, bool cond);
    void CALL_cond(Processor *proc, bool cond);
    void RST(Processor *proc, u8 addr);
}

#endif