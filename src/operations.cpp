#include "operations.h"
#include "processor.h"
#include <iostream>

void set_nhc_flags_add(Processor *proc, int a, int b)
{
    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_add(a, b));
    proc->set_flags(Processor::CARRY, utils::full_carry_add(a, b));
}

void set_nhc_flags_sub(Processor *proc, int a, int b)
{
    proc->set_flags(Processor::SUBTRACT, 1);
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_sub(a, b));
    proc->set_flags(Processor::CARRY, b > a);
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
    dest.set(proc->memory->read(src.value()));
}

void op::LD_mem(Processor *proc, r16 const &dest, r8 const &src)
{
    proc->memory->write(dest.value(), src.value());
}

void op::PUSH(Processor *proc, r8 const &reg)
{
    // Stack goes backwards, SP points to last element pushed so decrement first
    proc->SP.decrement();
    proc->memory->write(proc->SP.value(), reg.value());
}

void op::PUSH(Processor *proc, r16 const &reg)
{
    proc->SP.decrement();
    proc->memory->write(proc->SP.value(), reg.value_high());
    proc->SP.decrement();
    proc->memory->write(proc->SP.value(), reg.value_low());
}

void op::POP(Processor *proc, r8 &reg)
{
    reg.set(proc->memory->read(proc->SP.value()));
    proc->SP.increment();
}

void op::POP(Processor *proc, r16 &reg)
{
    reg.set_low(proc->memory->read(proc->SP.value()));
    proc->SP.increment();
    reg.set_high(proc->memory->read(proc->SP.value()));
    proc->SP.increment();
}

void op::ADD(Processor *proc, r8 &dest, r8 const &src)
{  
    set_nhc_flags_add(proc, dest.value(), src.value());
    dest.add(src.value());
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::ADD(Processor *proc, r16 &dest, r16 const &src)
{
    // zero flag not affected, carry checks for overflow
    bool c = dest.value() + src.value() > 0xffff;
    proc->set_flags(Processor::CARRY, c);
    // half carry checks for carry from bit 11
    bool hc = (((dest.value() & 0xfff) + (src.value() & 0xfff)) & 0x1000) == 0x1000;
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::SUBTRACT, 0);
    dest.add(src.value());
}

void op::ADD_imm(Processor *proc, r8 &reg)
{
    u8 add = proc->fetch_byte();
    set_nhc_flags_add(proc, reg.value(), add);
    reg.add(add);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::ADD_imm(Processor *proc, r16 &reg)
{
    // signed 16 bit operand
    i8 add = (i8)proc->fetch_byte();
    set_nhc_flags_add(proc, reg.value(), add);
    // zero flag always 0 for 16-bit immediate add
    proc->set_flags(Processor::ZERO, 0);
    reg.add(add);
}

void op::ADD_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 add = proc->memory->read(src.value());
    set_nhc_flags_add(proc, dest.value(), add);
    dest.add(add);
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::ADC(Processor *proc, r8 &dest, r8 const &src)
{
    // carry flag checks for overflow
    bool c = dest.value() + src.value() + proc->carry_flag() > 0xff;
    // check for half carry in either addition
    bool hc = utils::half_carry_add(src.value(), dest.value());
    dest.add(src.value());
    hc |= utils::half_carry_add(dest.value(), proc->carry_flag());
    dest.add(proc->carry_flag());

    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::ADC_imm(Processor *proc, r8 &reg)
{
    u8 add = proc->fetch_byte();
    bool c = reg.value() + add + proc->carry_flag() > 0xff;
    bool hc = utils::half_carry_add(reg.value(), add);
    reg.add(add);
    hc |= utils::half_carry_add(reg.value(), proc->carry_flag());
    reg.add(proc->carry_flag());

    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::ZERO, reg.value() == 0);   
}

void op::ADC_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 add = proc->memory->read(src.value());
    bool c = dest.value() + add + proc->carry_flag() > 0xff;
    bool hc = utils::half_carry_add(dest.value(), add);
    dest.add(add);
    hc |= utils::half_carry_add(dest.value(), proc->carry_flag());
    dest.add(proc->carry_flag());

    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::ZERO, dest.value() == 0);

}

void op::SUB(Processor *proc, r8 &dest, r8 const &src)
{
    set_nhc_flags_sub(proc, dest.value(), src.value());
    dest.sub(src.value());
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::SUB_imm(Processor *proc, r8 &reg)
{
    u8 sub = proc->fetch_byte();
    set_nhc_flags_sub(proc, reg.value(), sub);
    reg.sub(sub);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::SUB_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 sub = proc->memory->read(src.value());
    set_nhc_flags_sub(proc, dest.value(), sub);
    dest.sub(sub);
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::SBC(Processor *proc, r8 &dest, r8 const &src)
{
    bool c = dest.value() < src.value() + proc->carry_flag();
    bool hc = utils::half_carry_sub(dest.value(), src.value());
    dest.sub(src.value());
    hc |= utils::half_carry_sub(dest.value(), proc->carry_flag());
    dest.sub(proc->carry_flag());

    proc->set_flags(Processor::ZERO, dest.value() == 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::SUBTRACT, 1);
}

void op::SBC_imm(Processor *proc, r8 &reg)
{
    u8 sub = proc->fetch_byte();
    bool c = reg.value() < sub + proc->carry_flag();
    bool hc = utils::half_carry_sub(reg.value(), sub);
    reg.sub(sub);
    hc |= utils::half_carry_sub(reg.value(), proc->carry_flag());
    reg.sub(proc->carry_flag());

    proc->set_flags(Processor::ZERO, reg.value() == 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::SUBTRACT, 1);
}

void op::SBC_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 sub = proc->memory->read(src.value());
    bool c = dest.value() < sub + proc->carry_flag();
    bool hc = utils::half_carry_sub(dest.value(), sub);
    dest.sub(sub);
    hc |= utils::half_carry_sub(dest.value(), proc->carry_flag());
    dest.sub(proc->carry_flag());

    proc->set_flags(Processor::ZERO, dest.value() == 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::SUBTRACT, 1);
}

void op::INC(Processor *proc, r8 &reg)
{
    // carry flag unaffected    
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_add(reg.value(), 1));
    proc->set_flags(Processor::SUBTRACT, 0);
    reg.increment();
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::INC(r16 &reg)
{
    // no flags affected by 16-bit inc
    reg.increment();
}

void op::INC_mem(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory->read(reg.value());
    // carry flag unaffected
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_add(val, 1));
    proc->set_flags(Processor::SUBTRACT, 0);
    proc->memory->write(reg.value(), val + 1);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::DEC(Processor *proc, r8 &reg)
{
    // carry flag unaffected
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_sub(reg.value(), 1));
    //proc->set_flags(Processor::HALF_CARRY, 1 > reg.value());
    proc->set_flags(Processor::SUBTRACT, 1);
    reg.decrement();
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::DEC(Processor *proc, r16 &reg)
{
    // no flags affected by 16-bit dec
    reg.decrement();
}

void op::DEC_mem(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory->read(reg.value());
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_sub(val, 1));
    proc->set_flags(Processor::SUBTRACT, 1);
    proc->memory->write(reg.value(), val - 1);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::AND(Processor *proc, r8 &dest, r8 const &src)
{
    dest.set(dest.value() & src.value());
    proc->set_flags(Processor::SUBTRACT | Processor::CARRY, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::AND_imm(Processor *proc, r8 &reg)
{
    u8 val = proc->fetch_byte();
    reg.set(reg.value() & val);
    proc->set_flags(Processor::SUBTRACT | Processor::CARRY, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::AND_mem(Processor *proc, r8 &dest, r16 const &src)
{
    dest.set(dest.value() & proc->memory->read(src.value()));
    proc->set_flags(Processor::SUBTRACT | Processor::CARRY, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::OR(Processor *proc, r8 &dest, r8 const &src)
{
    dest.set(dest.value() | src.value());
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::OR_imm(Processor *proc, r8 &reg)
{
    reg.set(reg.value() | proc->fetch_byte());
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::OR_mem(Processor *proc, r8 &dest, r16 const &src)
{
    dest.set(dest.value() | proc->memory->read(src.value()));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::XOR(Processor *proc, r8 &dest, r8 const &src)
{
    dest.set(dest.value() ^ src.value());
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::XOR_imm(Processor *proc, r8 &reg)
{
    reg.set(reg.value() ^ proc->fetch_byte());
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::XOR_mem(Processor *proc, r8 &dest, r16 const &src)
{
    dest.set(dest.value() ^ proc->memory->read(src.value()));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, dest.value() == 0);
}

void op::CP(Processor *proc, r8 &dest, r8 const &src)
{
    proc->set_flags(Processor::ZERO, dest.value() == src.value());
     set_nhc_flags_sub(proc, dest.value(), src.value());
}

void op::CP_imm(Processor *proc, r8 &reg)
{
    u8 val = proc->fetch_byte();
    proc->set_flags(Processor::ZERO, reg.value() - val == 0);
    set_nhc_flags_sub(proc, reg.value(), val);  
}

void op::CP_mem(Processor *proc, r8 &dest, r16 const &src)
{
    u8 val = proc->memory->read(src.value());
    proc->set_flags(Processor::ZERO, dest.value() == val);
    set_nhc_flags_sub(proc, dest.value(), val);  
}

void op::SWAP(Processor *proc, r8 &reg)
{
    reg.set(utils::swap(reg.value()));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::SWAP_mem(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory->read(reg.value());
    proc->memory->write(reg.value(), utils::swap(val));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::RL(Processor *proc, r8 &reg)
{
    u8 bit7 = reg.value() >> 7;
    // rotate through carry
    reg.set((reg.value() << 1) | (u8)proc->carry_flag());
    proc->set_flags(Processor::CARRY, bit7);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::RL(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory->read(reg.value());
    // rotate through carry
    proc->memory->write(reg.value(), (val << 1) | (u8)proc->carry_flag());
    // old bit 7 to carry
    proc->set_flags(Processor::CARRY, (val >> 7) & 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
    
}

void op::RLC(Processor *proc, r8 &reg)
{
    bool bit7 = (reg.value() >> 7) & 1;
    // simple rotate
    reg.set((reg.value() << 1) | bit7);
    proc->set_flags(Processor::CARRY, bit7);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::RLC(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory->read(reg.value());
    bool bit7 = (val >> 7) & 1;
    // simple rotate
    proc->memory->write(reg.value(), (val << 1) | bit7);
    // old bit 7 to carry
    proc->set_flags(Processor::CARRY, bit7);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::RR(Processor *proc, r8 &reg)
{
    u8 bit0 = reg.value() & 1;
    // rotate through carry
    reg.set((reg.value() >> 1) | (proc->carry_flag() << 7));
    proc->set_flags(Processor::CARRY, bit0);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::RR(Processor *proc, r16 const &reg)
{
    // rotate through carry
    u8 val = proc->memory->read(reg.value());
    proc->memory->write(reg.value(), (val >> 1) | (proc->carry_flag() << 7));
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, val & 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::RRC(Processor *proc, r8 &reg)
{
    u8 bit0 = reg.value() & 1;
    // simple rotate
    reg.set((reg.value() >> 1) | (bit0 << 7));
    proc->set_flags(Processor::CARRY, bit0);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::RRC(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory->read(reg.value());
    // simple rotate
    proc->memory->write(reg.value(), (val >> 1) | (val << 7));
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, val & 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::SLA(Processor *proc, r8 &reg)
{
    // old bit 7 to carry
    proc->set_flags(Processor::CARRY, reg.value() >> 7);
    reg.set(reg.value() << 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::SLA(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory->read(reg.value());
    proc->memory->write(reg.value(), val << 1);
    // old bit 7 to carry
    proc->set_flags(Processor::CARRY, val >> 7);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::SRA(Processor *proc, r8 &reg)
{
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, reg.value() & 1);
    // shift bit 7 in from right
    reg.set((reg.value() >> 1) | (reg.value() & (1 << 7)));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::SRA(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory->read(reg.value());
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, val & 1);
    // shift bit 7 in from right
    proc->memory->write(reg.value(), (val >> 1) | (val & (1 << 7)));
    // carry always set to 0
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::SRL(Processor *proc, r8 &reg)
{
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, reg.value() & 1);
    // shift 0 in from right
    reg.set(reg.value() >> 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg.value() == 0);
}

void op::SRL(Processor *proc, r16 const &reg)
{
    u8 val = proc->memory->read(reg.value());
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, val & 1);
    proc->memory->write(reg.value(), val >> 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::BIT(Processor *proc, r8 &reg, u8 bit)
{
    proc->set_flags(Processor::ZERO, ((reg.value() >> bit) & 1) == 0);
    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
}

void op::BIT(Processor *proc, r16 const &reg, u8 bit)
{
    u8 val = proc->memory->read(reg.value());
    proc->set_flags(Processor::ZERO, ((val >> bit) & 1) == 0);
    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
}

void op::SET(r8 &reg, u8 bit)
{
    reg.set(reg.value() | (1 << bit));
}

void op::SET(Processor *proc, r16 const &reg, u8 bit)
{
    u8 val = proc->memory->read(reg.value());
    proc->memory->write(reg.value(), val | (1 << bit));
}

void op::RES(r8 &reg, u8 bit)
{
    reg.set(reg.value() & (~(1 << bit)));
}

void op::RES(Processor *proc, r16 const &reg, u8 bit)
{
    u8 val = proc->memory->read(reg.value());
    proc->memory->write(reg.value(), val & (~(1 << bit)));
}

void op::JP(Processor *proc, bool cond)
{
    u16 addr = proc->fetch_word();
    if (cond) {
        proc->PC.set(addr);
    }
}

void op::JP(Processor *proc, r16 &reg)
{
    proc->PC.set(reg.value());
}

void op::JR(Processor *proc, bool cond)
{
    // Since PC is advances after fetching instruction and argument, the jump 
    // is jp + 2 bytes ahead of instruction address
    i8 jp = (i8)proc->fetch_byte();
    if (cond) {
        proc->PC.add(jp);
    }
}

void op::CALL(Processor *proc, bool cond)
{
    u16 addr = proc->fetch_word();
    if (cond) {
        PUSH(proc, proc->PC);
        proc->PC.set(addr);
    }
}

void op::RET(Processor *proc, bool cond)
{
    if (cond) {
        POP(proc, proc->PC);
    }
}

void op::RST(Processor *proc, u8 addr)
{
    PUSH(proc, proc->PC);
    proc->PC.set(addr);
}

void op::DAA(Processor *proc)
{
    // DAA - retroactively convert binary add/sub to BCD
    if (!proc->subtract_flag()) {
        if (proc->carry_flag() || proc->A.value() > 0x99) {
            proc->A.add(0x60);
            proc->set_flags(Processor::CARRY, 1);
        }
        if (proc->half_carry_flag() || (proc->A.value() & 0x0f) > 0x09) {
            proc->A.add(0x6);
        }
    } else {
        if (proc->carry_flag()) {
            proc->A.sub(0x60);
        }
        if (proc->half_carry_flag()) {
            proc->A.sub(0x6);
        }
    }
    proc->set_flags(Processor::ZERO, proc->A.value() == 0);
    proc->set_flags(Processor::HALF_CARRY, 0);
}

void op::CPL(Processor *proc)
{
    proc->A.set(~proc->A.value());
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 1);
}

void op::SCF(Processor *proc)
{
    proc->set_flags(Processor::CARRY, 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
}

void op::CCF(Processor *proc)
{
    proc->set_flags(Processor::CARRY, !proc->carry_flag());
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
}