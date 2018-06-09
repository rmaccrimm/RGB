#include "operations.h"
#include "processor.h"

void op::set_nhc_flags_add(Processor *proc, int a, int b)
{
    proc->set_flag(Processor::SUBTRACT, 0);
    proc->set_flag(Processor::HALF_CARRY, utils::half_carry_add(a, b));
    proc->set_flag(Processor::CARRY, utils::full_carry_add(a, b));
}

void op::set_nhc_flags_sub(Processor *proc, int a, int b)
{
    proc->set_flag(Processor::SUBTRACT, 1);
    proc->set_flag(Processor::HALF_CARRY, utils::half_carry_sub(a, b));
    proc->set_flag(Processor::CARRY, utils::full_carry_sub(a, b));

}

void op::NOP() {}
void op::INVALID() {}
void op::HALT() {}
void op::STOP() {}

void op::LD(r8 &dest, r8 const &src)
{
    dest.set(src.value());
}

void op::LD(r16 &dest, r16 const &src)
{
    dest.set(src.value());
}

void op::LD_imm(Processor *proc, r8 &reg)
{
    reg.set(proc->fetch_byte());
}

void op::LD_imm(Processor *proc, r16 &reg)
{
    reg.set(proc->fetch_word());
}

void op::LD_mem(Processor *proc, r8 &dest, r16 const &src)
{
    dest.set(proc->memory.read(src.value()));
}

void op::LD_mem(Processor *proc, r16 const &dest, r8 const &src)
{
    proc->memory.write(dest.value(), src.value());
}

void op::PUSH(Processor *proc, r8 const &reg)
{
    // Stack goes backwards, SP points to last element pushed so decrement first
    proc->SP.decrement();
    proc->memory.write(proc->SP.value(), reg.value());
}

void op::PUSH(Processor *proc, r16 const &reg)
{
    proc->SP.decrement();
    proc->memory.write(proc->SP.value(), reg.value_high());
    proc->SP.decrement();
    proc->memory.write(proc->SP.value(), reg.value_low());
}

void op::POP(Processor *proc, r8 &reg)
{
    reg.set(proc->memory.read(proc->SP.value()));
    proc->SP.increment();
}

void op::POP(Processor *proc, r16 &reg)
{
    reg.set_low(proc->memory.read(proc->SP.value()));
    proc->SP.increment();
    reg.set_high(proc->memory.read(proc->SP.value()));
    proc->SP.increment();
}

void op::ADD(Processor *proc, r8 &dest, r8 const &src)
{  
    set_nhc_flags_add(proc, dest.value(), src.value());
    dest.add(src.value());
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::ADD(Processor *proc, r16 &dest, r16 const &src)
{
    // zero flag not affected for 16-bit add
    set_nhc_flags_add(proc, dest.value(), src.value());
    dest.add(src.value());
}

void op::ADD_imm(Processor *proc, r8 &reg)
{
    u8 add = proc->fetch_byte();
    set_nhc_flags_add(proc, reg.value(), add);
    reg.add(add);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::ADD_imm(Processor *proc, r16 &reg)
{
    u16 add = proc->fetch_word();
    set_nhc_flags_add(proc, reg.value(), add);
    // zero flag always 0 for 16-bit immediate add
    proc->set_flag(Processor::ZERO, 0);
    reg.add(add);
}

void op::ADD_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 add = proc->memory.read(src.value());
    set_nhc_flags_add(proc, dest.value(), add);
    dest.add(add);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::ADC(Processor *proc, r8 &dest, r8 const &src)
{
    // Not sure about setting flags with carry
    set_nhc_flags_add(proc, dest.value(), src.value() + proc->carry_flag());
    dest.add(src.value() + proc->carry_flag());
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::ADC_imm(Processor *proc, r8 &reg)
{
    u8 add = proc->fetch_byte() + proc->carry_flag();
    set_nhc_flags_add(proc, reg.value(), add);
    reg.add(add);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::ADC_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 add = proc->memory.read(src.value()) + proc->carry_flag();
    set_nhc_flags_add(proc, dest.value(), add);
    dest.add(add);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::SUB(Processor *proc, r8 &dest, r8 const &src)
{
    set_nhc_flags_sub(proc, dest.value(), src.value());
    dest.sub(src.value());
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::SUB_imm(Processor *proc, r8 &reg)
{
    u8 sub = proc->fetch_byte();
    set_nhc_flags_sub(proc, reg.value(), sub);
    reg.sub(sub);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::SUB_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 sub = proc->memory.read(src.value());
    set_nhc_flags_sub(proc, dest.value(), sub);
    dest.sub(sub);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::SBC(Processor *proc, r8 &dest, r8 const &src)
{
    set_nhc_flags_sub(proc, dest.value(), src.value() + proc->carry_flag());
    dest.sub(src.value() + proc->carry_flag());
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::SBC_imm(Processor *proc, r8 &reg)
{
    u8 sub = proc->fetch_byte() + proc->carry_flag();
    set_nhc_flags_sub(proc, reg.value(), sub);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::SBC_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 sub = proc->memory.read(src.value()) + proc->carry_flag();
    set_nhc_flags_sub(proc, dest.value(), sub);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::INC(Processor *proc, r8 &reg)
{
    // carry flag unaffected    
    proc->set_flag(Processor::HALF_CARRY, utils::half_carry_add(reg.value(), 1));
    proc->set_flag(Processor::SUBTRACT, 0);
    reg.increment();
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::INC(r16 &reg)
{
    // no flags affected by 16-bit inc
    reg.increment();
}

void op::INC_mem(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory.read(reg.value());
    // carry flag unaffected
    proc->set_flag(Processor::HALF_CARRY, utils::half_carry_add(val, 1));
    proc->set_flag(Processor::SUBTRACT, 0);
    proc->memory.write(reg.value(), val + 1);
    proc->set_flag(Processor::ZERO, val + 1 == 0);
}

void op::DEC(Processor *proc, r8 &reg)
{
    // carry flag unaffected
    proc->set_flag(Processor::HALF_CARRY, utils::half_carry_sub(reg.value(), 1));
    proc->set_flag(Processor::SUBTRACT, 1);
    reg.decrement();
    proc->set_flag(Processor::SUBTRACT, reg.value() == 0);
}

void op::DEC(Processor *proc, r16 &reg)
{
    // no flags affected by 16-bit dec
    reg.decrement();
}

void op::DEC_mem(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory.read(reg.value());
    proc->set_flag(Processor::HALF_CARRY, utils::half_carry_sub(val, 1));
    proc->set_flag(Processor::SUBTRACT, 1);
    proc->memory.write(reg.value(), val - 1);
    proc->set_flag(Processor::ZERO, val - 1 == 0);
}

void op::AND(Processor *proc, r8 &dest, r8 const &src)
{
    dest.set(dest.value() & src.value());
    proc->set_flag(Processor::SUBTRACT | Processor::CARRY, 0);
    proc->set_flag(Processor::HALF_CARRY, 1);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::AND_imm(Processor *proc, r8 &reg)
{
    u8 and = proc->fetch_byte();
    reg.set(reg.value() & and);
    proc->set_flag(Processor::SUBTRACT | Processor::CARRY, 0);
    proc->set_flag(Processor::HALF_CARRY, 1);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::AND_mem(Processor *proc, r8 &dest, r16 const &src)
{
    dest.set(dest.value() & proc->memory.read(src.value()));
    proc->set_flag(Processor::SUBTRACT | Processor::CARRY, 0);
    proc->set_flag(Processor::HALF_CARRY, 1);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::OR(Processor *proc, r8 &dest, r8 const &src)
{
    dest.set(dest.value() | src.value());
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::OR_imm(Processor *proc, r8 &reg)
{
    reg.set(reg.value() | proc->fetch_byte());
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::OR_mem(Processor *proc, r8 &dest, r16 const &src)
{
    dest.set(dest.value() | proc->memory.read(src.value()));
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::XOR(Processor *proc, r8 &dest, r8 const &src)
{
    dest.set(dest.value() ^ src.value());
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::XOR_imm(Processor *proc, r8 &reg)
{
    reg.set(reg.value() ^ proc->fetch_byte());
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::XOR_mem(Processor *proc, r8 &dest, r16 const &src)
{
    dest.set(dest.value() ^ proc->memory.read(src.value()));
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flag(Processor::ZERO, dest.value() == 0);
}

void op::CP(Processor *proc, r8 &dest, r8 const &src)
{
    proc->set_flag(Processor::ZERO, dest.value() == src.value());
    // dest - src computed internally, so same flags as sub
    set_nhc_flags_sub(proc, src.value(), dest.value());    
}

void op::CP_imm(Processor *proc, r8 &reg)
{
    u8 val = proc->fetch_byte();
    proc->set_flag(Processor::ZERO, reg.value() == val);
    set_nhc_flags_sub(proc, reg.value(), val);  
}

void op::CP_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 val = proc->memory.read(src.value());
    proc->set_flag(Processor::ZERO, dest.value() == val);
    set_nhc_flags_sub(proc, dest.value(), val);  
}

void op::SWAP(Processor *proc, r8 &reg)
{
    reg.set(utils::swap(reg.value()));
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::SWAP_mem(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory.read(reg.value());
    proc->memory.write(reg.value(), utils::swap(val));
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    val = proc->memory.read(reg.value());
    proc->set_flag(Processor::ZERO, val == 0);
}

void op::RL(Processor *proc, r8 &reg)
{
    u8 bit7 = reg.value();
    // rotate through carry
    reg.set((reg.value() << 1) | proc->carry_flag());
    proc->set_flag(Processor::CARRY, bit7);
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::RL(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory.read(reg.value());
    // rotate through carry
    proc->memory.write(reg.value(), (val << 1) | proc->carry_flag());
    // old bit 7 to carry
    proc->set_flag(Processor::CARRY, (val >> 7) & 1);
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory.read(reg.value());
    proc->set_flag(Processor::ZERO, val == 0);
    
}

void op::RLC(Processor *proc, r8 &reg)
{
    u8 bit7 = (reg.value() >> 7) & 1;
    // simple rotate
    reg.set((reg.value() << 1) | bit7);
    proc->set_flag(Processor::CARRY, bit7);
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::RLC(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory.read(reg.value());
    // simple rotate
    proc->memory.write(reg.value(), (val << 1) | ((val >> 7) & 1));
    // old bit 7 to carry
    proc->set_flag(Processor::CARRY, (val >> 7) & 1);
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::RR(Processor *proc, r8 &reg)
{
    u8 bit0 = reg.value() & 1;
    // rotate through carry
    reg.set((reg.value() >> 1) | (proc->carry_flag() << 7));
    proc->set_flag(Processor::CARRY, bit0);
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::RR(Processor *proc, r16 const &reg)
{
    // rotate through carry
    u8 val = proc->memory.read(reg.value());
    proc->memory.write(reg.value(), (val >> 1) | (proc->carry_flag() << 7));
    // old bit 0 to carry
    proc->set_flag(Processor::CARRY, val & 1);
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory.read(reg.value());
    proc->set_flag(Processor::ZERO, val == 0);
}

void op::RRC(Processor *proc, r8 &reg)
{
    u8 bit0 = reg.value() & 1;
    // simple rotate
    reg.set((reg.value() >> 1) | (bit0 << 7));
    proc->set_flag(Processor::CARRY, bit0);
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

void op::RRC(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory.read(reg.value());
    // simple rotate
    proc->memory.write(reg.value(), (val >> 1) | (val << 7));
    // old bit 0 to carry
    proc->set_flag(Processor::CARRY, val & 1);
    proc->set_flag(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flag(Processor::ZERO, reg.value() == 0);
}

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

void JP(Processor *proc, bool cond);
void JP(Processor *proc, r16 &reg);
void JR(Processor *proc, bool cond);
void CALL(Processor *proc, bool cond);
void RET(Processor *proc, bool cond);
void RETI(Processor *proc);

void RST(Processor *proc, u8 addr);
void DAA(Processor *proc);
void CPL(Processor *proc);
void SCF(Processor *proc);
void CCF(Processor *proc);
void EI(Processor *proc);
void DI(Processor *proc);