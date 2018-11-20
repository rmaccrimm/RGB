#include "operations.h"
#include "processor.h"

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
void op::STOP() {}

void op::LD(u8 &dest, u8 &src)
{
    dest = src;
}

void op::LD(reg16 &dest, reg16 &src)
{
    dest.value = src.value;
}

void op::LD_imm(Processor *proc, u8 &reg)
{
    reg = (proc->fetch_byte());
}

void op::LD_imm(Processor *proc, reg16 &reg)
{
    reg.value = (proc->fetch_word());
}

void op::LD_mem(Processor *proc, u8 &dest, reg16 &src)
{
    dest = (proc->memory->read(src.value));
}

void op::LD_mem(Processor *proc, reg16 &dest, u8 &src)
{
    proc->memory->write(dest.value, src);
}

void op::PUSH(Processor *proc, u8 &reg)
{
    // Stack goes backwards, SP points to last element pushed so decrement first
    proc->SP.value--;
    proc->memory->write(proc->SP.value, reg);
}

void op::PUSH(Processor *proc, reg16 &reg)
{
    proc->SP.value--;
    proc->memory->write(proc->SP.value, reg.high);
    proc->SP.value--;
    proc->memory->write(proc->SP.value, reg.low);
}

void op::POP(Processor *proc, u8 &reg)
{
    reg = (proc->memory->read(proc->SP.value));
    proc->SP.value++;
}

void op::POP(Processor *proc, reg16 &reg)
{
    reg.low = (proc->memory->read(proc->SP.value));
    proc->SP.value++;
    reg.high = (proc->memory->read(proc->SP.value));
    proc->SP.value++;
}

void op::ADD(Processor *proc, u8 &dest, u8 &src)
{  
    set_nhc_flags_add(proc, dest, src);
    dest += src;
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::ADD(Processor *proc, reg16 &dest, reg16 &src)
{
    // zero flag not affected, carry checks for overflow
    bool c = dest.value + src.value > 0xffff;
    proc->set_flags(Processor::CARRY, c);
    // half carry checks for carry from bit 11
    bool hc = (((dest.value & 0xfff) + (src.value & 0xfff)) & 0x1000) == 0x1000;
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::SUBTRACT, 0);
    dest.value += src.value;
}

void op::ADD_imm(Processor *proc, u8 &reg)
{
    u8 add = proc->fetch_byte();
    set_nhc_flags_add(proc, reg, add);
    reg += (add);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::ADD_imm(Processor *proc, reg16 &reg)
{
    // signed 16 bit operand
    i8 add = (i8)proc->fetch_byte();
    set_nhc_flags_add(proc, reg.value, add);
    // zero flag always 0 for 16-bit immediate add
    proc->set_flags(Processor::ZERO, 0);
    reg.value += add;
}

void op::ADD_mem(Processor *proc, u8 &dest, reg16 &src)
{
    u8 add = proc->memory->read(src.value);
    set_nhc_flags_add(proc, dest, add);
    dest += (add);
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::ADC(Processor *proc, u8 &dest, u8 &src)
{
    // carry flag checks for overflow
    bool c = (u16)dest + src + proc->carry_flag() > 0xff;
    // check for half carry in either addition
    bool hc = utils::half_carry_add(src, dest);
    dest += (src);
    hc |= utils::half_carry_add(dest, proc->carry_flag());
    dest += (proc->carry_flag());

    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::ADC_imm(Processor *proc, u8 &reg)
{
    u8 add = proc->fetch_byte();
    bool c = (u16)reg + add + proc->carry_flag() > 0xff;
    bool hc = utils::half_carry_add(reg, add);
    reg += (add);
    hc |= utils::half_carry_add(reg, proc->carry_flag());
    reg += (proc->carry_flag());

    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::ZERO, reg == 0);   
}

void op::ADC_mem(Processor *proc, u8 &dest, reg16 &src)
{
    u8 add = proc->memory->read(src.value);
    bool c = (u16)dest + add + proc->carry_flag() > 0xff;
    bool hc = utils::half_carry_add(dest, add);
    dest += (add);
    hc |= utils::half_carry_add(dest, proc->carry_flag());
    dest += (proc->carry_flag());

    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::ZERO, dest == 0);

}

void op::SUB(Processor *proc, u8 &dest, u8 &src)
{
    set_nhc_flags_sub(proc, dest, src);
    dest -= src;
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::SUB_imm(Processor *proc, u8 &reg)
{
    u8 sub = proc->fetch_byte();
    set_nhc_flags_sub(proc, reg, sub);
    reg -= sub;
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::SUB_mem(Processor *proc, u8 &dest, reg16 &src)
{
    u8 sub = proc->memory->read(src.value);
    set_nhc_flags_sub(proc, dest, sub);
    dest -= sub;
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::SBC(Processor *proc, u8 &dest, u8 &src)
{
    bool c = dest < src + proc->carry_flag();
    bool hc = utils::half_carry_sub(dest, src);
    dest -= (src);
    hc |= utils::half_carry_sub(dest, proc->carry_flag());
    dest -= (proc->carry_flag());

    proc->set_flags(Processor::ZERO, dest == 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::SUBTRACT, 1);
}

void op::SBC_imm(Processor *proc, u8 &reg)
{
    u8 sub = proc->fetch_byte();
    bool c = reg < sub + proc->carry_flag();
    bool hc = utils::half_carry_sub(reg, sub);
    reg -= (sub);
    hc |= utils::half_carry_sub(reg, proc->carry_flag());
    reg -= (proc->carry_flag());

    proc->set_flags(Processor::ZERO, reg == 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::SUBTRACT, 1);
}

void op::SBC_mem(Processor *proc, u8 &dest, reg16 &src)
{
    u8 sub = proc->memory->read(src.value);
    bool c = dest < sub + proc->carry_flag();
    bool hc = utils::half_carry_sub(dest, sub);
    dest -= (sub);
    hc |= utils::half_carry_sub(dest, proc->carry_flag());
    dest -= (proc->carry_flag());

    proc->set_flags(Processor::ZERO, dest == 0);
    proc->set_flags(Processor::HALF_CARRY, hc);
    proc->set_flags(Processor::CARRY, c);
    proc->set_flags(Processor::SUBTRACT, 1);
}

void op::INC(Processor *proc, u8 &reg)
{
    // carry flag unaffected    
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_add(reg, 1));
    proc->set_flags(Processor::SUBTRACT, 0);
    reg++;
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::INC(reg16 &reg)
{
    // no flags affected by 16-bit inc
    reg.value++;
}

void op::INC_mem(Processor *proc, reg16 &reg)
{
    u8 val = proc->memory->read(reg.value);
    // carry flag unaffected
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_add(val, 1));
    proc->set_flags(Processor::SUBTRACT, 0);
    proc->memory->write(reg.value, val + 1);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::DEC(Processor *proc, u8 &reg)
{
    // carry flag unaffected
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_sub(reg, 1));
    //proc->set_flags(Processor::HALF_CARRY, 1 > reg);
    proc->set_flags(Processor::SUBTRACT, 1);
    reg--;
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::DEC(Processor *proc, reg16 &reg)
{
    // no flags affected by 16-bit dec
    reg.value--;
}

void op::DEC_mem(Processor *proc, reg16 &reg)
{
    u8 val = proc->memory->read(reg.value);
    proc->set_flags(Processor::HALF_CARRY, utils::half_carry_sub(val, 1));
    proc->set_flags(Processor::SUBTRACT, 1);
    proc->memory->write(reg.value, val - 1);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::AND(Processor *proc, u8 &dest, u8 &src)
{
    dest = (dest & src);
    proc->set_flags(Processor::SUBTRACT | Processor::CARRY, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::AND_imm(Processor *proc, u8 &reg)
{
    u8 val = proc->fetch_byte();
    reg = (reg & val);
    proc->set_flags(Processor::SUBTRACT | Processor::CARRY, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::AND_mem(Processor *proc, u8 &dest, reg16 &src)
{
    dest = (dest & proc->memory->read(src.value));
    proc->set_flags(Processor::SUBTRACT | Processor::CARRY, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::OR(Processor *proc, u8 &dest, u8 &src)
{
    dest = (dest | src);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::OR_imm(Processor *proc, u8 &reg)
{
    reg = (reg | proc->fetch_byte());
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::OR_mem(Processor *proc, u8 &dest, reg16 &src)
{
    dest = (dest | proc->memory->read(src.value));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::XOR(Processor *proc, u8 &dest, u8 &src)
{
    dest = (dest ^ src);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::XOR_imm(Processor *proc, u8 &reg)
{
    reg = (reg ^ proc->fetch_byte());
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::XOR_mem(Processor *proc, u8 &dest, reg16 &src)
{
    dest = (dest ^ proc->memory->read(src.value));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, dest == 0);
}

void op::CP(Processor *proc, u8 &dest, u8 &src)
{
    proc->set_flags(Processor::ZERO, dest == src);
     set_nhc_flags_sub(proc, dest, src);
}

void op::CP_imm(Processor *proc, u8 &reg)
{
    u8 val = proc->fetch_byte();
    proc->set_flags(Processor::ZERO, reg - val == 0);
    set_nhc_flags_sub(proc, reg, val);  
}

void op::CP_mem(Processor *proc, u8 &dest, reg16 &src)
{
    u8 val = proc->memory->read(src.value);
    proc->set_flags(Processor::ZERO, dest == val);
    set_nhc_flags_sub(proc, dest, val);  
}

void op::SWAP(Processor *proc, u8 &reg)
{
    reg = (utils::swap(reg));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::SWAP_mem(Processor *proc, reg16 &reg)
{
    u8 val = proc->memory->read(reg.value);
    proc->memory->write(reg.value, utils::swap(val));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY | Processor::CARRY, 0);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::RL(Processor *proc, u8 &reg)
{
    u8 bit7 = reg >> 7;
    // rotate through carry
    reg = ((reg << 1) | (u8)proc->carry_flag());
    proc->set_flags(Processor::CARRY, bit7);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::RL(Processor *proc, reg16 &reg)
{
    u8 val = proc->memory->read(reg.value);
    // rotate through carry
    proc->memory->write(reg.value, (val << 1) | (u8)proc->carry_flag());
    // old bit 7 to carry
    proc->set_flags(Processor::CARRY, (val >> 7) & 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
    
}

void op::RLC(Processor *proc, u8 &reg)
{
    bool bit7 = (reg >> 7) & 1;
    // simple rotate
    reg = ((reg << 1) | (u8)bit7);
    proc->set_flags(Processor::CARRY, bit7);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::RLC(Processor *proc, reg16 &reg)
{
    u8 val = proc->memory->read(reg.value);
    bool bit7 = (val >> 7) & 1;
    // simple rotate
    proc->memory->write(reg.value, (val << 1) | (u8)bit7);
    // old bit 7 to carry
    proc->set_flags(Processor::CARRY, bit7);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::RR(Processor *proc, u8 &reg)
{
    u8 bit0 = reg & 1;
    // rotate through carry
    reg = ((reg >> 1) | (proc->carry_flag() << 7));
    proc->set_flags(Processor::CARRY, bit0);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::RR(Processor *proc, reg16 &reg)
{
    // rotate through carry
    u8 val = proc->memory->read(reg.value);
    proc->memory->write(reg.value, (val >> 1) | (proc->carry_flag() << 7));
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, val & 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::RRC(Processor *proc, u8 &reg)
{
    u8 bit0 = reg & 1;
    // simple rotate
    reg = ((reg >> 1) | (bit0 << 7));
    proc->set_flags(Processor::CARRY, bit0);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::RRC(Processor *proc, reg16 &reg)
{
    u8 val = proc->memory->read(reg.value);
    // simple rotate
    proc->memory->write(reg.value, (val >> 1) | (val << 7));
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, val & 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::SLA(Processor *proc, u8 &reg)
{
    // old bit 7 to carry
    proc->set_flags(Processor::CARRY, reg >> 7);
    reg = (reg << 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::SLA(Processor *proc, reg16 &reg)
{
    u8 val = proc->memory->read(reg.value);
    proc->memory->write(reg.value, val << 1);
    // old bit 7 to carry
    proc->set_flags(Processor::CARRY, val >> 7);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::SRA(Processor *proc, u8 &reg)
{
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, reg & 1);
    // shift bit 7 in from right
    reg = ((reg >> 1) | (reg & (1 << 7)));
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::SRA(Processor *proc, reg16 &reg)
{
    u8 val = proc->memory->read(reg.value);
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, val & 1);
    // shift bit 7 in from right
    proc->memory->write(reg.value, (val >> 1) | (val & (1 << 7)));
    // carry always set to 0
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::SRL(Processor *proc, u8 &reg)
{
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, reg & 1);
    // shift 0 in from right
    reg = (reg >> 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    proc->set_flags(Processor::ZERO, reg == 0);
}

void op::SRL(Processor *proc, reg16 &reg)
{
    u8 val = proc->memory->read(reg.value);
    // old bit 0 to carry
    proc->set_flags(Processor::CARRY, val & 1);
    proc->memory->write(reg.value, val >> 1);
    proc->set_flags(Processor::SUBTRACT | Processor::HALF_CARRY, 0);
    val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, val == 0);
}

void op::BIT(Processor *proc, u8 &reg, u8 bit)
{
    proc->set_flags(Processor::ZERO, ((reg >> bit) & 1) == 0);
    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
}

void op::BIT(Processor *proc, reg16 &reg, u8 bit)
{
    u8 val = proc->memory->read(reg.value);
    proc->set_flags(Processor::ZERO, ((val >> bit) & 1) == 0);
    proc->set_flags(Processor::SUBTRACT, 0);
    proc->set_flags(Processor::HALF_CARRY, 1);
}

void op::SET(u8 &reg, u8 bit)
{
    reg = (utils::set(reg, bit));
}

void op::SET(Processor *proc, reg16 &reg, u8 bit)
{
    u8 val = proc->memory->read(reg.value);
    proc->memory->write(reg.value, utils::set(val, bit));
}

void op::RES(u8 &reg, u8 bit)
{
    reg = (utils::reset(reg, bit));
}

void op::RES(Processor *proc, reg16 &reg, u8 bit)
{
    u8 val = proc->memory->read(reg.value);
    proc->memory->write(reg.value, utils::reset(val, bit));
}

void op::JP(Processor *proc, bool cond)
{
    u16 addr = proc->fetch_word();
    if (cond) {
        proc->PC.value = addr;
    }
}

void op::JP(Processor *proc, reg16 &reg)
{
    proc->PC = (reg);
}

void op::JR(Processor *proc, bool cond)
{
    // Since PC is advances after fetching instruction and argument, the jump 
    // is jp + 2 bytes ahead of instruction address
    i8 jp = (i8)proc->fetch_byte();
    if (cond) {
        proc->PC.value += (jp);
    }
}

void op::CALL(Processor *proc, bool cond)
{
    u16 addr = proc->fetch_word();
    if (cond) {
        PUSH(proc, proc->PC);
        proc->PC.value = addr;
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
    proc->PC.value = addr;
}

void op::DAA(Processor *proc)
{
    // DAA - retroactively convert binary add/sub to BCD
    if (!proc->subtract_flag()) {
        if (proc->carry_flag() || proc->A > 0x99) {
            proc->A += (0x60);
            proc->set_flags(Processor::CARRY, 1);
        }
        if (proc->half_carry_flag() || (proc->A & 0x0f) > 0x09) {
            proc->A += (0x6);
        }
    } else {
        if (proc->carry_flag()) {
            proc->A -= (0x60);
        }
        if (proc->half_carry_flag()) {
            proc->A -= (0x6);
        }
    }
    proc->set_flags(Processor::ZERO, proc->A == 0);
    proc->set_flags(Processor::HALF_CARRY, 0);
}

void op::CPL(Processor *proc)
{
    proc->A = (~proc->A);
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