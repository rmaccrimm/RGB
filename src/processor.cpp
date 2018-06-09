#include "processor.h"
#include "debug.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cassert>

bool Processor::step(int break_point)
{
    u8 instr = fetch_byte();
    execute(instr);
    process_interrupts();
    
    if (DEBUG_MODE) {
        std::cout << std::hex << (int)instr << std::endl;
        //print_register_values();
    }
    if (PC.value() == break_point) {
        return false;
    }
}

void Processor::print_registers()
{
    std::cout << "AF:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)AF.value() << "\n"
              << "BC:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)BC.value() << "\n"
              << "DE:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)DE.value() << "\n"
              << "HL:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)HL.value() << "\n"
              << "SP:\t"  <<  std::setw(4) << std::setfill('0')
              << std::hex << (int)SP.value() << "\n"
              << "PC:\t"  << std::setw(4) << std::setfill('0')
              << std::hex << (int)PC.value() << "\n";
}

u8 Processor::fetch_byte()
{
    u8 data = memory.read(PC.value());
    PC.increment();
    return data;
}

u16 Processor::fetch_word()
{
    u8 low = fetch_byte();
    u8 high = fetch_byte();
    u16 word = (high << 8) | low;
    return word;
}

void Processor::set_flag(u8 mask, bool b)
{
    if (b) {
        F.set(F.value() | mask);
    } else {
        F.set(F.value() & ~mask);
    }
}

bool Processor::carry_flag() { return F.value() & ZERO; }

bool Processor::subtract_flag() { return F.value() & SUBTRACT; }

bool Processor::half_carry_flag() { return F.value() & HALF_CARRY; }

bool Processor::carry_flag() { return F.value() & CARRY; }


void Processor::execute(u8 instr)
{
    switch(instr)
    {
    case 0x00:
        op::NOP();                             
        break;
    case 0x01:
        op::LD_imm(this, BC);                  
        break;
    case 0x02:
        op::LD_mem(this, BC, A);               
        break;
    case 0x03:
        op::INC(this, BC);                     
        break;
    case 0x04:
        op::INC(this, B);                      
        break;
    case 0x05:
        op::DEC(this, B);                      
        break;
    case 0x06:
        op::LD_imm(this, B);                   
        break;
    case 0x07:
        op::RLC(this, A);                      
        break;
    case 0x08:
        
        break;
    case 0x09:
        op::ADD(this, HL, BC);                 
        break;
    case 0x0a:
        op::LD_mem(this, A, BC);               
        break;
    case 0x0b:
        op::DEC(this, BC);                     
        break;
    case 0x0c:
        op::INC(this, C);                      
        break;
    case 0x0d:
        op::DEC(this, C);                      
        break;
    case 0x0e:
        op::LD_imm(this, C);                   
        break;
    case 0x0f:
        op::RRC(this, A);                      
        break;
    case 0x10:
        op::STOP();                            
        break;
    case 0x11:
        op::LD_imm(this, DE);                  
        break;
    case 0x12:
        op::LD_mem(this, DE, A);               
        break;
    case 0x13:
        op::INC(this, DE);                     
        break;
    case 0x14:
        op::INC(this, D);                      
        break;
    case 0x15:
        op::DEC(this, D);                      
        break;
    case 0x16:
        op::LD_imm(this, D);                   
        break;
    case 0x17:
        op::RL(this, A);                       
        break;
    case 0x18:
        op::JR(this, true);
        break;
    case 0x19:
        op::ADD(this, HL, DE);                 
        break;
    case 0x1a:
        op::LD_mem(this, A, DE);               
        break;
    case 0x1b:
        op::DEC(this, DE);                     
        break;
    case 0x1c:
        op::INC(this, E);                      
        break;
    case 0x1d:
        op::DEC(this, E);                      
        break;
    case 0x1e:
        op::LD_imm(this, E);                   
        break;
    case 0x1f:
        op::RR(this, A);                       
        break;
    case 0x20:
        op::JR(this, !(flags() & ZERO));
        break;
    case 0x21:
        op::LD_imm(this, HL);                  
        break;
    case 0x22:
        
        break;
    case 0x23:
        op::INC(this, HL);                     
        break;
    case 0x24:
        op::INC(this, H);                      
        break;
    case 0x25:
        op::DEC(this, H);                      
        break;
    case 0x26:
        op::LD_imm(this, H);                   
        break;
    case 0x27:
        op::DAA(this);                         
        break;
    case 0x28:
        op::JR(this, flags() & ZERO);
        break;
    case 0x29:
        op::ADD(this, HL, HL);                 
        break;
    case 0x2a:
        
        break;
    case 0x2b:
        op::DEC(this, HL);                     
        break;
    case 0x2c:
        op::INC(this, L);                      
        break;
    case 0x2d:
        op::DEC(this, L);                      
        break;
    case 0x2e:
        op::LD_imm(this, L);                   
        break;
    case 0x2f:
        op::CPL(this);                         
        break;
    case 0x30:
        op::JR(this, !(flags() & CARRY));                        
        break;
    case 0x31:
        op::LD_imm(this, SP);                  
        break;
    case 0x32:
        op::LD_mem(this, HL, A);
        HL.decrement();
        break;
    case 0x33:
        op::INC(this, SP);                     
        break;
    case 0x34:
        op::INC_mem(this, HL);                 
        break;
    case 0x35:
        op::DEC_mem(this, HL);
        break;
    case 0x36:
        
        break;
    case 0x37:
        op::SCF(this);                         
        break;
    case 0x38:
        op::JR(this, flags() & CARRY);
        break;
    case 0x39:
        op::ADD(this, HL, SP);    
        break;
    case 0x3a:
        
        break;
    case 0x3b:
        op::DEC(this, SP);                     
        break;
    case 0x3c:
        op::INC(this, A);                      
        break;
    case 0x3d:
        op::DEC(this, A);                      
        break;
    case 0x3e:
        op::LD_imm(this, A);                   
        break;
    case 0x3f:
        op::CCF(this);                         
        break;
    case 0x40:
        op::LD(B, B);                    
        break;
    case 0x41:
        op::LD(B, C);                    
        break;
    case 0x42:
        op::LD(B, D);                    
        break;
    case 0x43:
        op::LD(B, E);                    
        break;
    case 0x44:
        op::LD(B, H);                    
        break;
    case 0x45:
        op::LD(B, L);                    
        break;
    case 0x46:
        op::LD_mem(this, B, HL);               
        break;
    case 0x47:
        op::LD(B, A);                    
        break;
    case 0x48:
        op::LD(C, B);                    
        break;
    case 0x49:
        op::LD(C, C);                    
        break;
    case 0x4a:
        op::LD(C, D);                    
        break;
    case 0x4b:
        op::LD(C, E);                    
        break;
    case 0x4c:
        op::LD(C, H);                    
        break;
    case 0x4d:
        op::LD(C, L);                    
        break;
    case 0x4e:
        op::LD_mem(this, C, HL);               
        break;
    case 0x4f:
        op::LD(C, A);                    
        break;
    case 0x50:
        op::LD(D, B);                    
        break;
    case 0x51:
        op::LD(D, C);                    
        break;
    case 0x52:
        op::LD(D, D);                    
        break;
    case 0x53:
        op::LD(D, E);                    
        break;
    case 0x54:
        op::LD(D, H);                    
        break;
    case 0x55:
        op::LD(D, L);                    
        break;
    case 0x56:
        op::LD_mem(this, D, HL);               
        break;
    case 0x57:
        op::LD(D, A);                    
        break;
    case 0x58:
        op::LD(E, B);                    
        break;
    case 0x59:
        op::LD(E, C);                    
        break;
    case 0x5a:
        op::LD(E, D);                    
        break;
    case 0x5b:
        op::LD(E, E);                    
        break;
    case 0x5c:
        op::LD(E, H);                    
        break;
    case 0x5d:
        op::LD(E, L);                    
        break;
    case 0x5e:
        op::LD_mem(this, E, HL);               
        break;
    case 0x5f:
        op::LD(E, A);                    
        break;
    case 0x60:
        op::LD(H, B);                    
        break;
    case 0x61:
        op::LD(H, C);                    
        break;
    case 0x62:
        op::LD(H, D);                    
        break;
    case 0x63:
        op::LD(H, E);                    
        break;
    case 0x64:
        op::LD(H, H);                    
        break;
    case 0x65:
        op::LD(H, L);                    
        break;
    case 0x66:
        op::LD_mem(this, H, HL);               
        break;
    case 0x67:
        op::LD(H, A);                    
        break;
    case 0x68:
        op::LD(L, B);                    
        break;
    case 0x69:
        op::LD(L, C);                    
        break;
    case 0x6a:
        op::LD(L, D);                    
        break;
    case 0x6b:
        op::LD(L, E);                    
        break;
    case 0x6c:
        op::LD(L, H);                    
        break;
    case 0x6d:
        op::LD(L, L);                    
        break;
    case 0x6e:
        op::LD_mem(this, L, HL);               
        break;
    case 0x6f:
        op::LD(L, A);                    
        break;
    case 0x70:
        op::LD_mem(this, HL, B);               
        break;
    case 0x71:
        op::LD_mem(this, HL, C);               
        break;
    case 0x72:
        op::LD_mem(this, HL, D);               
        break;
    case 0x73:
        op::LD_mem(this, HL, E);               
        break;
    case 0x74:
        op::LD_mem(this, HL, H);               
        break;
    case 0x75:
        op::LD_mem(this, HL, L);               
        break;
    case 0x76:
        op::HALT();                        
        break;
    case 0x77:
        op::LD_mem(this, HL, A);               
        break;
    case 0x78:
        op::LD(A, B);                    
        break;
    case 0x79:
        op::LD(A, C);                    
        break;
    case 0x7a:
        op::LD(A, D);                    
        break;
    case 0x7b:
        op::LD(A, E);                    
        break;
    case 0x7c:
        op::LD(A, H);                    
        break;
    case 0x7d:
        op::LD(A, L);                    
        break;
    case 0x7e:
        op::LD_mem(this, A, HL);               
        break;
    case 0x7f:
        op::LD(A, A);                    
        break;
    case 0x80:
        op::ADD(this, A, B);                   
        break;
    case 0x81:
        op::ADD(this, A, C);                   
        break;
    case 0x82:
        op::ADD(this, A, D);                   
        break;
    case 0x83:
        op::ADD(this, A, E);                   
        break;
    case 0x84:
        op::ADD(this, A, H);                   
        break;
    case 0x85:
        op::ADD(this, A, L);                   
        break;
    case 0x86:
        op::ADD_mem(this, A, HL);              
        break;
    case 0x87:
        op::ADD(this, A, A);                   
        break;
    case 0x88:
        op::ADC(this, A, B);                   
        break;
    case 0x89:
        op::ADC(this, A, C);                   
        break;
    case 0x8a:
        op::ADC(this, A, D);                   
        break;
    case 0x8b:
        op::ADC(this, A, E);                   
        break;
    case 0x8c:
        op::ADC(this, A, H);                   
        break;
    case 0x8d:
        op::ADC(this, A, L);                   
        break;
    case 0x8e:
        op::ADC_mem(this, A, HL);              
        break;
    case 0x8f:
        op::ADC(this, A, A);                   
        break;
    case 0x90:
        op::SUB(this, A, B);                   
        break;
    case 0x91:
        op::SUB(this, A, C);                   
        break;
    case 0x92:
        op::SUB(this, A, D);                   
        break;
    case 0x93:
        op::SUB(this, A, E);                   
        break;
    case 0x94:
        op::SUB(this, A, H);                   
        break;
    case 0x95:
        op::SUB(this, A, L);                   
        break;
    case 0x96:
        op::SUB_mem(this, A, HL);              
        break;
    case 0x97:
        op::SBC(this, A, A);                   
        break;
    case 0x98:
        op::SBC(this, A, B);                   
        break;
    case 0x99:
        op::SBC(this, A, C);                   
        break;
    case 0x9a:
        op::SBC(this, A, D);                   
        break;
    case 0x9b:
        op::SBC(this, A, E);                   
        break;
    case 0x9c:
        op::SBC(this, A, H);                   
        break;
    case 0x9d:
        op::SBC(this, A, L);                   
        break;
    case 0x9e:
        op::SBC_mem(this, A, HL);              
        break;
    case 0x9f:
        op::SBC(this, A, A);                   
        break;
    case 0xa0:
        op::AND(this, A, B);                   
        break;
    case 0xa1:
        op::AND(this, A, C);                   
        break;
    case 0xa2:
        op::AND(this, A, D);                   
        break;
    case 0xa3:
        op::AND(this, A, E);                   
        break;
    case 0xa4:
        op::AND(this, A, H);                   
        break;
    case 0xa5:
        op::AND(this, A, L);                   
        break;
    case 0xa6:
        op::AND_mem(this, A, HL);              
        break;
    case 0xa7:
        op::AND(this, A, A);                   
        break;
    case 0xa8:
        op::XOR(this, A, B);                   
        break;
    case 0xa9:
        op::XOR(this, A, C);                   
        break;
    case 0xaa:
        op::XOR(this, A, D);                   
        break;
    case 0xab:
        op::XOR(this, A, E);                   
        break;
    case 0xac:
        op::XOR(this, A, H);                   
        break;
    case 0xad:
        op::XOR(this, A, L);                   
        break;
    case 0xae:
        op::XOR_mem(this, A, HL);              
        break;
    case 0xaf:
        op::XOR(this, A, A);                   
        break;
    case 0xb0:
        op::OR(this, A, B);;                   
        break;
    case 0xb1:
        op::OR(this, A, C);;                   
        break;
    case 0xb2:
        op::OR(this, A, D);;                   
        break;
    case 0xb3:
        op::OR(this, A, E);;                   
        break;
    case 0xb4:
        op::OR(this, A, H);;                   
        break;
    case 0xb5:
        op::OR(this, A, L);;                   
        break;
    case 0xb6:
        op::OR_mem(this, A, HL);;              
        break;
    case 0xb7:
        op::OR(this, A, A);;                   
        break;
    case 0xb8:
        op::CP(this, A, B);;                   
        break;
    case 0xb9:
        op::CP(this, A, C);;                   
        break;
    case 0xba:
        op::CP(this, A, D);;                   
        break;
    case 0xbb:
        op::CP(this, A, E);;                   
        break;
    case 0xbc:
        op::CP(this, A, H);;                   
        break;
    case 0xbd:
        op::CP(this, A, L);;                   
        break;
    case 0xbe:
        op::CP_mem(this, A, HL);;              
        break;
    case 0xbf:
        op::CP(this, A, A);;                   
        break;
    case 0xc0:
        op::RET(this, !(flags() & ZERO));
        break;
    case 0xc1:
        op::POP(this, BC);                     
        break;
    case 0xc2:
        op::JP(this, !(flags() & ZERO));
        break;
    case 0xc3:
        op::JP(this, true);
        break;
    case 0xc4:
        op::CALL(this, !(flags() & ZERO));                      
        break;
    case 0xc5:
        op::PUSH(this, BC);                    
        break;
    case 0xc6:
        op::ADD_imm(this, A);                  
        break;
    case 0xc7:
        op::RST(this, 0x00);                   
        break;
    case 0xc8:
        op::RET(this, flags() & ZERO);
        break;
    case 0xc9:
        op::RET(this, true);
        break;
    case 0xca:
        op::JP(this, flags() & ZERO);
        break;
    case 0xcb:
        instr = fetch_byte();
        cb_execute(instr);
        break;
    case 0xcc:
        op::CALL(this, flags() & ZERO);                       
        break;
    case 0xcd:
        op::CALL(this, true);
        break;
    case 0xce:
        op::ADC_imm(this, A);                  
        break;
    case 0xcf:
        op::RST(this, 0x08);                   
        break;
    case 0xd0:
        op::RET(this, !(flags() & CARRY));
        break;
    case 0xd1:
        op::POP(this, DE);                     
        break;
    case 0xd2:
        op::JP(this, !(flags() & CARRY));
        break;
    case 0xd3:
        op::INVALID();                         
        break;
    case 0xd4:
        op::CALL(this, !(flags() & CARRY));
        break;
    case 0xd5:
        op::PUSH(this, DE);                    
        break;
    case 0xd6:
        op::SUB_imm(this, A);
        break;
    case 0xd7:
        op::RST(this, 0x10);                   
        break;
    case 0xd8:
        op::RET(this, flags() & CARRY);
        break;
    case 0xd9:
        op::RETI(this);                        
        break;
    case 0xda:
        op::JP(this, flags() & CARRY);
        break;
    case 0xdb:
        op::INVALID();                         
        break;
    case 0xdc:
        op::CALL(this, flags() & CARRY);
        break;
    case 0xdd:
        op::INVALID();                         
        break;
    case 0xde:
        op::SBC_imm(this, A);                  
        break;
    case 0xdf:
        op::RST(this, 0x18);                   
        break;
    case 0xe0:
        memory.write(0xff00 + fetch_byte(), A.value());
        break;
    case 0xe1:
        op::POP(this, HL);       
        break;
    case 0xe2:
                    
        break;
    case 0xe3:
        op::INVALID();                         
        break;
    case 0xe4:
        op::INVALID();                         
        break;
    case 0xe5:
        op::PUSH(this, HL);                    
        break;
    case 0xe6:
        op::AND_imm(this, A);
        break;
    case 0xe7:
        op::RST(this, 0x20);                   
        break;
    case 0xe8:
        op::ADD_imm(this, SP);                 
        break;
    case 0xe9:
        op::JP(this, HL);
        break;
    case 0xea:
        
        break;
    case 0xeb:
        op::INVALID();                         
        break;
    case 0xec:
        op::INVALID();                         
        break;
    case 0xed:
        op::INVALID();                         
        break;
    case 0xee:
        op::XOR_imm(this, A);
        break;
    case 0xef:
        op::RST(this, 0x28);                   
        break;
    case 0xf0:
        A.set(memory.read(0xff00 + fetch_byte()));
        break;
    case 0xf1:
        op::POP(this, AF);                     
        break;
    case 0xf2:
        A.set(memory.read(0xff00 + C.value()));
        break;
    case 0xf3:
        op::DI(this);                              
        break;
    case 0xf4:
        op::INVALID();                         
        break;
    case 0xf5:
        op::PUSH(this, AF);                    
        break;
    case 0xf6:
        op::OR_imm(this, A);                   
        break;
    case 0xf7:
        op::RST(this, 0x30);        
        break;
    case 0xf8:
        
        break;
    case 0xf9:
        op::LD(SP, HL); 
        break;
    case 0xfa:
        
        break;
    case 0xfb:
        op::EI(this);                              
        break;
    case 0xfc:
        op::INVALID();                         
        break;
    case 0xfd:
        op::INVALID();                         
        break;
    case 0xfe:
        op::CP_imm(this, A);
        break;
    case 0xff:
        op::RST(this, 0x38);    
        break;
    default:
        // TODO ?
    }
}

void Processor::cb_execute(u8 instr)
{
    switch(instr)
    {
    case 0x00:
        op::RLC(this, B);    
        break;
    case 0x01:
        op::RLC(this, C);    
        break;
    case 0x02:
        op::RLC(this, D);    
        break;
    case 0x03:
        op::RLC(this, E);    
        break;
    case 0x04:
        op::RLC(this, H);    
        break;
    case 0x05:
        op::RLC(this, L);    
        break;
    case 0x06:
        op::RLC(this, HL);   
        break;
    case 0x07:
        op::RLC(this, A);    
        break;
    case 0x08:
        op::RRC(this, B);    
        break;
    case 0x09:
        op::RRC(this, C);    
        break;
    case 0x0a:
        op::RRC(this, D);    
        break;
    case 0x0b:
        op::RRC(this, E);    
        break;
    case 0x0c:
        op::RRC(this, H);    
        break;
    case 0x0d:
        op::RRC(this, L);    
        break;
    case 0x0e:
        op::RRC(this, HL);   
        break;
    case 0x0f:
        op::RRC(this, A);    
        break;
    case 0x10:
        op::RL(this, B);     
        break;
    case 0x11:
        op::RL(this, C);     
        break;
    case 0x12:
        op::RL(this, D);     
        break;
    case 0x13:
        op::RL(this, E);     
        break;
    case 0x14:
        op::RL(this, H);     
        break;
    case 0x15:
        op::RL(this, L);     
        break;
    case 0x16:
        op::RL(this, HL);    
        break;
    case 0x17:
        op::RL(this, A);     
        break;
    case 0x18:
        op::RR(this, B);     
        break;
    case 0x19:
        op::RR(this, C);     
        break;
    case 0x1a:
        op::RR(this, D);     
        break;
    case 0x1b:
        op::RR(this, E);     
        break;
    case 0x1c:
        op::RR(this, H);     
        break;
    case 0x1d:
        op::RR(this, L);     
        break;
    case 0x1e:
        op::RR(this, HL);    
        break;
    case 0x1f:
        op::RR(this, A);     
        break;
    case 0x20:
        op::SLA(this, B);    
        break;
    case 0x21:
        op::SLA(this, C);    
        break;
    case 0x22:
        op::SLA(this, D);    
        break;
    case 0x23:
        op::SLA(this, E);    
        break;
    case 0x24:
        op::SLA(this, H);    
        break;
    case 0x25:
        op::SLA(this, L);    
        break;
    case 0x26:
        op::SLA(this, HL);   
        break;
    case 0x27:
        op::SLA(this, A);    
        break;
    case 0x28:
        op::SRA(this, B);    
        break;
    case 0x29:
        op::SRA(this, C);    
        break;
    case 0x2a:
        op::SRA(this, D);    
        break;
    case 0x2b:
        op::SRA(this, E);    
        break;
    case 0x2c:
        op::SRA(this, H);    
        break;
    case 0x2d:
        op::SRA(this, L);    
        break;
    case 0x2e:
        op::SRA(this, HL);   
        break;
    case 0x2f:
        op::SRA(this, A);    
        break;
    case 0x30:
        op::SWAP(this, B);   
        break;
    case 0x31:
        op::SWAP(this, C);   
        break;
    case 0x32:
        op::SWAP(this, D);   
        break;
    case 0x33:
        op::SWAP(this, E);   
        break;
    case 0x34:
        op::SWAP(this, H);   
        break;
    case 0x35:
        op::SWAP(this, L);   
        break;
    case 0x36:
        op::SWAP(this, HL);  
        break;
    case 0x37:
        op::SWAP(this, A);   
        break;
    case 0x38:
        op::SRL(this, B);    
        break;
    case 0x39:
        op::SRL(this, C);    
        break;
    case 0x3a:
        op::SRL(this, D);    
        break;
    case 0x3b:
        op::SRL(this, E);    
        break;
    case 0x3c:
        op::SRL(this, H);    
        break;
    case 0x3d:
        op::SRL(this, L);    
        break;
    case 0x3e:
        op::SRL(this, HL);   
        break;
    case 0x3f:
        op::SRL(this, A);    
        break;
    case 0x40:
        op::BIT(this, B, 0);  
        break;
    case 0x41:
        op::BIT(this, C, 0);  
        break;
    case 0x42:
        op::BIT(this, D, 0);  
        break;
    case 0x43:
        op::BIT(this, E, 0);  
        break;
    case 0x44:
        op::BIT(this, H, 0);  
        break;
    case 0x45:
        op::BIT(this, L, 0);  
        break;
    case 0x46:
        op::BIT(this, HL, 0); 
        break;
    case 0x47:
        op::BIT(this, A, 0);  
        break;
    case 0x48:
        op::BIT(this, B, 1);  
        break;
    case 0x49:
        op::BIT(this, C, 1);  
        break;
    case 0x4a:
        op::BIT(this, D, 1);  
        break;
    case 0x4b:
        op::BIT(this, E, 1);  
        break;
    case 0x4c:
        op::BIT(this, H, 1);  
        break;
    case 0x4d:
        op::BIT(this, L, 1);  
        break;
    case 0x4e:
        op::BIT(this, HL, 1); 
        break;
    case 0x4f:
        op::BIT(this, A, 1);  
        break;
    case 0x50:
        op::BIT(this, B, 2);  
        break;
    case 0x51:
        op::BIT(this, C, 2);  
        break;
    case 0x52:
        op::BIT(this, D, 2);  
        break;
    case 0x53:
        op::BIT(this, E, 2);  
        break;
    case 0x54:
        op::BIT(this, H, 2);  
        break;
    case 0x55:
        op::BIT(this, L, 2);  
        break;
    case 0x56:
        op::BIT(this, HL, 2); 
        break;
    case 0x57:
        op::BIT(this, A, 2);  
        break;
    case 0x58:
        op::BIT(this, B, 3);  
        break;
    case 0x59:
        op::BIT(this, C, 3);  
        break;
    case 0x5a:
        op::BIT(this, D, 3);  
        break;
    case 0x5b:
        op::BIT(this, E, 3);  
        break;
    case 0x5c:
        op::BIT(this, H, 3);  
        break;
    case 0x5d:
        op::BIT(this, L, 3);  
        break;
    case 0x5e:
        op::BIT(this, HL, 3); 
        break;
    case 0x5f:
        op::BIT(this, A, 3);  
        break;
    case 0x60:
        op::BIT(this, B, 4);  
        break;
    case 0x61:
        op::BIT(this, C, 4);  
        break;
    case 0x62:
        op::BIT(this, D, 4);  
        break;
    case 0x63:
        op::BIT(this, E, 4);  
        break;
    case 0x64:
        op::BIT(this, H, 4);  
        break;
    case 0x65:
        op::BIT(this, L, 4);  
        break;
    case 0x66:
        op::BIT(this, HL, 4); 
        break;
    case 0x67:
        op::BIT(this, A, 4);  
        break;
    case 0x68:
        op::BIT(this, B, 5);  
        break;
    case 0x69:
        op::BIT(this, C, 5);  
        break;
    case 0x6a:
        op::BIT(this, D, 5);  
        break;
    case 0x6b:
        op::BIT(this, E, 5);  
        break;
    case 0x6c:
        op::BIT(this, H, 5);  
        break;
    case 0x6d:
        op::BIT(this, L, 5);  
        break;
    case 0x6e:
        op::BIT(this, HL, 5); 
        break;
    case 0x6f:
        op::BIT(this, A, 5);  
        break;
    case 0x70:
        op::BIT(this, B, 6);  
        break;
    case 0x71:
        op::BIT(this, C, 6);  
        break;
    case 0x72:
        op::BIT(this, D, 6);  
        break;
    case 0x73:
        op::BIT(this, E, 6);  
        break;
    case 0x74:
        op::BIT(this, H, 6);  
        break;
    case 0x75:
        op::BIT(this, L, 6);  
        break;
    case 0x76:
        op::BIT(this, HL, 6); 
        break;
    case 0x77:
        op::BIT(this, A, 6);  
        break;
    case 0x78:
        op::BIT(this, B, 7);  
        break;
    case 0x79:
        op::BIT(this, C, 7);  
        break;
    case 0x7a:
        op::BIT(this, D, 7);  
        break;
    case 0x7b:
        op::BIT(this, E, 7);  
        break;
    case 0x7c:
        op::BIT(this, H, 7);  
        break;
    case 0x7d:
        op::BIT(this, L, 7);  
        break;
    case 0x7e:
        op::BIT(this, HL, 7); 
        break;
    case 0x7f:
        op::BIT(this, A, 7);  
        break;
    case 0x80:
        op::RES(this, B, 0);  
        break;
    case 0x81:
        op::RES(this, C, 0);  
        break;
    case 0x82:
        op::RES(this, D, 0);  
        break;
    case 0x83:
        op::RES(this, E, 0);  
        break;
    case 0x84:
        op::RES(this, H, 0);  
        break;
    case 0x85:
        op::RES(this, L, 0);  
        break;
    case 0x86:
        op::RES(this, HL, 0); 
        break;
    case 0x87:
        op::RES(this, A, 0);  
        break;
    case 0x88:
        op::RES(this, B, 1);  
        break;
    case 0x89:
        op::RES(this, C, 1);  
        break;
    case 0x8a:
        op::RES(this, D, 1);  
        break;
    case 0x8b:
        op::RES(this, E, 1);  
        break;
    case 0x8c:
        op::RES(this, H, 1);  
        break;
    case 0x8d:
        op::RES(this, L, 1);  
        break;
    case 0x8e:
        op::RES(this, HL, 1); 
        break;
    case 0x8f:
        op::RES(this, A, 1);  
        break;
    case 0x90:
        op::RES(this, B, 2);  
        break;
    case 0x91:
        op::RES(this, C, 2);  
        break;
    case 0x92:
        op::RES(this, D, 2);  
        break;
    case 0x93:
        op::RES(this, E, 2);  
        break;
    case 0x94:
        op::RES(this, H, 2);  
        break;
    case 0x95:
        op::RES(this, L, 2);  
        break;
    case 0x96:
        op::RES(this, HL, 2); 
        break;
    case 0x97:
        op::RES(this, A, 2);  
        break;
    case 0x98:
        op::RES(this, B, 3);  
        break;
    case 0x99:
        op::RES(this, C, 3);  
        break;
    case 0x9a:
        op::RES(this, D, 3);  
        break;
    case 0x9b:
        op::RES(this, E, 3);  
        break;
    case 0x9c:
        op::RES(this, H, 3);  
        break;
    case 0x9d:
        op::RES(this, L, 3);  
        break;
    case 0x9e:
        op::RES(this, HL, 3); 
        break;
    case 0x9f:
        op::RES(this, A, 3);  
        break;
    case 0xa0:
        op::RES(this, B, 4);  
        break;
    case 0xa1:
        op::RES(this, C, 4);  
        break;
    case 0xa2:
        op::RES(this, D, 4);  
        break;
    case 0xa3:
        op::RES(this, E, 4);  
        break;
    case 0xa4:
        op::RES(this, H, 4);  
        break;
    case 0xa5:
        op::RES(this, L, 4);  
        break;
    case 0xa6:
        op::RES(this, HL, 4); 
        break;
    case 0xa7:
        op::RES(this, A, 4);  
        break;
    case 0xa8:
        op::RES(this, B, 5);  
        break;
    case 0xa9:
        op::RES(this, C, 5);  
        break;
    case 0xaa:
        op::RES(this, D, 5);  
        break;
    case 0xab:
        op::RES(this, E, 5);  
        break;
    case 0xac:
        op::RES(this, H, 5);  
        break;
    case 0xad:
        op::RES(this, L, 5);  
        break;
    case 0xae:
        op::RES(this, HL, 5); 
        break;
    case 0xaf:
        op::RES(this, A, 5);  
        break;
    case 0xb0:
        op::RES(this, B, 6);  
        break;
    case 0xb1:
        op::RES(this, C, 6);  
        break;
    case 0xb2:
        op::RES(this, D, 6);  
        break;
    case 0xb3:
        op::RES(this, E, 6);  
        break;
    case 0xb4:
        op::RES(this, H, 6);  
        break;
    case 0xb5:
        op::RES(this, L, 6);  
        break;
    case 0xb6:
        op::RES(this, HL, 6); 
        break;
    case 0xb7:
        op::RES(this, A, 6);  
        break;
    case 0xb8:
        op::RES(this, B, 7);  
        break;
    case 0xb9:
        op::RES(this, C, 7);  
        break;
    case 0xba:
        op::RES(this, D, 7);  
        break;
    case 0xbb:
        op::RES(this, E, 7);  
        break;
    case 0xbc:
        op::RES(this, H, 7);  
        break;
    case 0xbd:
        op::RES(this, L, 7);  
        break;
    case 0xbe:
        op::RES(this, HL, 7); 
        break;
    case 0xbf:
        op::RES(this, A, 7);  
        break;
    case 0xc0:
        op::SET(this, B, 0);  
        break;
    case 0xc1:
        op::SET(this, C, 0);  
        break;
    case 0xc2:
        op::SET(this, D, 0);  
        break;
    case 0xc3:
        op::SET(this, E, 0);  
        break;
    case 0xc4:
        op::SET(this, H, 0);  
        break;
    case 0xc5:
        op::SET(this, L, 0);  
        break;
    case 0xc6:
        op::SET(this, HL, 0); 
        break;
    case 0xc7:
        op::SET(this, A, 0);  
        break;
    case 0xc8:
        op::SET(this, B, 1);  
        break;
    case 0xc9:
        op::SET(this, C, 1);  
        break;
    case 0xca:
        op::SET(this, D, 1);  
        break;
    case 0xcb:
        op::SET(this, E, 1);  
        break;
    case 0xcc:
        op::SET(this, H, 1);  
        break;
    case 0xcd:
        op::SET(this, L, 1);  
        break;
    case 0xce:
        op::SET(this, HL, 1); 
        break;
    case 0xcf:
        op::SET(this, A, 1);  
        break;
    case 0xd0:
        op::SET(this, B, 2);  
        break;
    case 0xd1:
        op::SET(this, C, 2);  
        break;
    case 0xd2:
        op::SET(this, D, 2);  
        break;
    case 0xd3:
        op::SET(this, E, 2);  
        break;
    case 0xd4:
        op::SET(this, H, 2);  
        break;
    case 0xd5:
        op::SET(this, L, 2);  
        break;
    case 0xd6:
        op::SET(this, HL, 2); 
        break;
    case 0xd7:
        op::SET(this, A, 2);  
        break;
    case 0xd8:
        op::SET(this, B, 3);  
        break;
    case 0xd9:
        op::SET(this, C, 3);  
        break;
    case 0xda:
        op::SET(this, D, 3);  
        break;
    case 0xdb:
        op::SET(this, E, 3);  
        break;
    case 0xdc:
        op::SET(this, H, 3);  
        break;
    case 0xdd:
        op::SET(this, L, 3);  
        break;
    case 0xde:
        op::SET(this, HL, 3); 
        break;
    case 0xdf:
        op::SET(this, A, 3);  
        break;
    case 0xe0:
        op::SET(this, B, 4);  
        break;
    case 0xe1:
        op::SET(this, C, 4);  
        break;
    case 0xe2:
        op::SET(this, D, 4);  
        break;
    case 0xe3:
        op::SET(this, E, 4);  
        break;
    case 0xe4:
        op::SET(this, H, 4);  
        break;
    case 0xe5:
        op::SET(this, L, 4);  
        break;
    case 0xe6:
        op::SET(this, HL, 4); 
        break;
    case 0xe7:
        op::SET(this, A, 4);  
        break;
    case 0xe8:
        op::SET(this, B, 5);  
        break;
    case 0xe9:
        op::SET(this, C, 5);  
        break;
    case 0xea:
        op::SET(this, D, 5);  
        break;
    case 0xeb:
        op::SET(this, E, 5);  
        break;
    case 0xec:
        op::SET(this, H, 5);  
        break;
    case 0xed:
        op::SET(this, L, 5);  
        break;
    case 0xee:
        op::SET(this, HL, 5); 
        break;
    case 0xef:
        op::SET(this, A, 5);  
        break;
    case 0xf0:
        op::SET(this, B, 6);  
        break;
    case 0xf1:
        op::SET(this, C, 6);  
        break;
    case 0xf2:
        op::SET(this, D, 6);  
        break;
    case 0xf3:
        op::SET(this, E, 6);  
        break;
    case 0xf4:
        op::SET(this, H, 6);  
        break;
    case 0xf5:
        op::SET(this, L, 6);  
        break;
    case 0xf6:
        op::SET(this, HL, 6); 
        break;
    case 0xf7:
        op::SET(this, A, 6);  
        break;
    case 0xf8:
        op::SET(this, B, 7);  
        break;
    case 0xf9:
        op::SET(this, C, 7);  
        break;
    case 0xfa:
        op::SET(this, D, 7);  
        break;
    case 0xfb:
        op::SET(this, E, 7);  
        break;
    case 0xfc:
        op::SET(this, H, 7);  
        break;
    case 0xfd:
        op::SET(this, L, 7);  
        break;
    case 0xfe:
        op::SET(this, HL, 7); 
        break;
    case 0xff:
        op::SET(this, A, 7);  
        break;
    default:
        // TODO ?
    }
}