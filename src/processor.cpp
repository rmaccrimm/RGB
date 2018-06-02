#include "processor.h"
#include "debug.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cassert>

bool Processor::execute(u8 instr, bool cb)
{
    OpFunc *op;
    if (cb) 
        op = &cb_opcodes[0];
    else 
        op = &opcodes[0];
    if (op[instr] == nullptr)
        return false;
    (this->*op[instr])();
    return true;
}

bool Processor::step(int break_point)
{
    u8 instr = fetch_byte();
    std::string prefix = "";
    bool cb = (instr == 0xcb);
    if (cb) {
        prefix = "cb ";
        instr = fetch_byte();
    }
    if (!execute(instr, cb)) {
        std::cout << "Unimplemented instruction: " << prefix
                << std::setw(2) << std::setfill('0') << std::hex
                << (int)instr << std::endl;
        return false;
    }
    if (DEBUG_MODE) {
        std::cout << std::hex << (int)instr << std::endl;
        if (PC.value() == break_point) {
            return false;
        }
    }
    return true;
}

void Processor::set_memory(u8 *mem) { memory = mem; }

void Processor::print_register_values()
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

void Processor::set(int b) { F.set(F.value() | (1 << b)); }

void Processor::reset(int b) { F.set(F.value() & (~(1 << b))); }

void Processor::set_cond(int b, bool cond) 
{ 
    if (cond)
        set(b);
    else 
        reset(b);
} 

bool Processor::is_set(int b)
{
    return (F.value()) >> b & 1;
}

Processor::Processor(u8 *mem):
    A(), F(), B(), C(), D(), E(), H(), L(), AF(&A, &F), BC(&B, &C),	DE(&D, &E), HL(&H, &L), 
    memory(mem)
{
    for (unsigned int i = 0; i < 0x100; i++) {
        opcodes[i] = nullptr;
        cb_opcodes[i] = nullptr;
    }
    opcodes[0x00] = &Processor::opcode0x00;
    opcodes[0x01] = &Processor::opcode0x01;
    opcodes[0x02] = &Processor::opcode0x02;
    opcodes[0x03] = &Processor::opcode0x03;
    opcodes[0x04] = &Processor::opcode0x04;
    opcodes[0x05] = &Processor::opcode0x05;
    opcodes[0x06] = &Processor::opcode0x06;
    opcodes[0x07] = &Processor::opcode0x07;
    opcodes[0x08] = &Processor::opcode0x08;
    opcodes[0x09] = &Processor::opcode0x09;
    opcodes[0x0a] = &Processor::opcode0x0a;
    opcodes[0x0b] = &Processor::opcode0x0b;
    opcodes[0x0c] = &Processor::opcode0x0c;
    opcodes[0x0d] = &Processor::opcode0x0d;
    opcodes[0x0e] = &Processor::opcode0x0e;
    opcodes[0x0f] = &Processor::opcode0x0f;
    //opcodes[0x10] = &Processor::opcode0x10;
    opcodes[0x11] = &Processor::opcode0x11;
    opcodes[0x12] = &Processor::opcode0x12;
    opcodes[0x13] = &Processor::opcode0x13;
    opcodes[0x14] = &Processor::opcode0x14;
    opcodes[0x15] = &Processor::opcode0x15;
    opcodes[0x16] = &Processor::opcode0x16;
    opcodes[0x17] = &Processor::opcode0x17;
    //opcodes[0x18] = &Processor::opcode0x18;
    opcodes[0x19] = &Processor::opcode0x19;
    opcodes[0x1a] = &Processor::opcode0x1a;
    opcodes[0x1b] = &Processor::opcode0x1b;
    opcodes[0x1c] = &Processor::opcode0x1c;
    opcodes[0x1d] = &Processor::opcode0x1d;
    opcodes[0x1e] = &Processor::opcode0x1e;
    opcodes[0x1f] = &Processor::opcode0x1f;
    opcodes[0x20] = &Processor::opcode0x20;
    opcodes[0x21] = &Processor::opcode0x21;
    opcodes[0x22] = &Processor::opcode0x22;
    opcodes[0x23] = &Processor::opcode0x23;
    opcodes[0x24] = &Processor::opcode0x24;
    opcodes[0x25] = &Processor::opcode0x25;
    opcodes[0x26] = &Processor::opcode0x26;
    opcodes[0x27] = &Processor::opcode0x27;
    //opcodes[0x28] = &Processor::opcode0x28;
    opcodes[0x29] = &Processor::opcode0x29;
    opcodes[0x2a] = &Processor::opcode0x2a;
    opcodes[0x2b] = &Processor::opcode0x2b;
    opcodes[0x2c] = &Processor::opcode0x2c;
    opcodes[0x2d] = &Processor::opcode0x2d;
    opcodes[0x2e] = &Processor::opcode0x2e;
    //opcodes[0x2f] = &Processor::opcode0x2f;
    //opcodes[0x30] = &Processor::opcode0x30;
    opcodes[0x31] = &Processor::opcode0x31;
    opcodes[0x32] = &Processor::opcode0x32;
    opcodes[0x33] = &Processor::opcode0x33;
    opcodes[0x34] = &Processor::opcode0x34;
    opcodes[0x35] = &Processor::opcode0x35;
    opcodes[0x36] = &Processor::opcode0x36;
    //opcodes[0x37] = &Processor::opcode0x37;
    //opcodes[0x38] = &Processor::opcode0x38;
    opcodes[0x39] = &Processor::opcode0x39;
    opcodes[0x3a] = &Processor::opcode0x3a;
    opcodes[0x3b] = &Processor::opcode0x3b;
    opcodes[0x3c] = &Processor::opcode0x3c;
    opcodes[0x3d] = &Processor::opcode0x3d;
    opcodes[0x3e] = &Processor::opcode0x3e;
    //opcodes[0x3f] = &Processor::opcode0x3f;
    opcodes[0x40] = &Processor::opcode0x40;
    opcodes[0x41] = &Processor::opcode0x41;
    opcodes[0x42] = &Processor::opcode0x42;
    opcodes[0x43] = &Processor::opcode0x43;
    opcodes[0x44] = &Processor::opcode0x44;
    opcodes[0x45] = &Processor::opcode0x45;
    opcodes[0x46] = &Processor::opcode0x46;
    opcodes[0x47] = &Processor::opcode0x47;
    opcodes[0x48] = &Processor::opcode0x48;
    opcodes[0x49] = &Processor::opcode0x49;
    opcodes[0x4a] = &Processor::opcode0x4a;
    opcodes[0x4b] = &Processor::opcode0x4b;
    opcodes[0x4c] = &Processor::opcode0x4c;
    opcodes[0x4d] = &Processor::opcode0x4d;
    opcodes[0x4e] = &Processor::opcode0x4e;
    opcodes[0x4f] = &Processor::opcode0x4f;
    opcodes[0x50] = &Processor::opcode0x50;
    opcodes[0x51] = &Processor::opcode0x51;
    opcodes[0x52] = &Processor::opcode0x52;
    opcodes[0x53] = &Processor::opcode0x53;
    opcodes[0x54] = &Processor::opcode0x54;
    opcodes[0x55] = &Processor::opcode0x55;
    opcodes[0x56] = &Processor::opcode0x56;
    opcodes[0x57] = &Processor::opcode0x57;
    opcodes[0x58] = &Processor::opcode0x58;
    opcodes[0x59] = &Processor::opcode0x59;
    opcodes[0x5a] = &Processor::opcode0x5a;
    opcodes[0x5b] = &Processor::opcode0x5b;
    opcodes[0x5c] = &Processor::opcode0x5c;
    opcodes[0x5d] = &Processor::opcode0x5d;
    opcodes[0x5e] = &Processor::opcode0x5e;
    opcodes[0x5f] = &Processor::opcode0x5f;
    opcodes[0x60] = &Processor::opcode0x60;
    opcodes[0x61] = &Processor::opcode0x61;
    opcodes[0x62] = &Processor::opcode0x62;
    opcodes[0x63] = &Processor::opcode0x63;
    opcodes[0x64] = &Processor::opcode0x64;
    opcodes[0x65] = &Processor::opcode0x65;
    opcodes[0x66] = &Processor::opcode0x66;
    opcodes[0x67] = &Processor::opcode0x67;
    opcodes[0x68] = &Processor::opcode0x68;
    opcodes[0x69] = &Processor::opcode0x69;
    opcodes[0x6a] = &Processor::opcode0x6a;
    opcodes[0x6b] = &Processor::opcode0x6b;
    opcodes[0x6c] = &Processor::opcode0x6c;
    opcodes[0x6d] = &Processor::opcode0x6d;
    opcodes[0x6e] = &Processor::opcode0x6e;
    opcodes[0x6f] = &Processor::opcode0x6f;
    opcodes[0x70] = &Processor::opcode0x70;
    opcodes[0x71] = &Processor::opcode0x71;
    opcodes[0x72] = &Processor::opcode0x72;
    opcodes[0x73] = &Processor::opcode0x73;
    opcodes[0x74] = &Processor::opcode0x74;
    opcodes[0x75] = &Processor::opcode0x75;
    //opcodes[0x76] = &Processor::opcode0x76;
    opcodes[0x77] = &Processor::opcode0x77;
    opcodes[0x78] = &Processor::opcode0x78;
    opcodes[0x79] = &Processor::opcode0x79;
    opcodes[0x7a] = &Processor::opcode0x7a;
    opcodes[0x7b] = &Processor::opcode0x7b;
    opcodes[0x7c] = &Processor::opcode0x7c;
    opcodes[0x7d] = &Processor::opcode0x7d;
    opcodes[0x7e] = &Processor::opcode0x7e;
    opcodes[0x7f] = &Processor::opcode0x7f;
    opcodes[0x80] = &Processor::opcode0x80;
    opcodes[0x81] = &Processor::opcode0x81;
    opcodes[0x82] = &Processor::opcode0x82;
    opcodes[0x83] = &Processor::opcode0x83;
    opcodes[0x84] = &Processor::opcode0x84;
    opcodes[0x85] = &Processor::opcode0x85;
    opcodes[0x86] = &Processor::opcode0x86;
    opcodes[0x87] = &Processor::opcode0x87;
    opcodes[0x88] = &Processor::opcode0x88;
    opcodes[0x89] = &Processor::opcode0x89;
    opcodes[0x8a] = &Processor::opcode0x8a;
    opcodes[0x8b] = &Processor::opcode0x8b;
    opcodes[0x8c] = &Processor::opcode0x8c;
    opcodes[0x8d] = &Processor::opcode0x8d;
    opcodes[0x8e] = &Processor::opcode0x8e;
    opcodes[0x8f] = &Processor::opcode0x8f;
    opcodes[0x90] = &Processor::opcode0x90;
    opcodes[0x91] = &Processor::opcode0x91;
    opcodes[0x92] = &Processor::opcode0x92;
    opcodes[0x93] = &Processor::opcode0x93;
    opcodes[0x94] = &Processor::opcode0x94;
    opcodes[0x95] = &Processor::opcode0x95;
    opcodes[0x96] = &Processor::opcode0x96;
    opcodes[0x97] = &Processor::opcode0x97;
    opcodes[0x98] = &Processor::opcode0x98;
    opcodes[0x99] = &Processor::opcode0x99;
    opcodes[0x9a] = &Processor::opcode0x9a;
    opcodes[0x9b] = &Processor::opcode0x9b;
    opcodes[0x9c] = &Processor::opcode0x9c;
    opcodes[0x9d] = &Processor::opcode0x9d;
    opcodes[0x9e] = &Processor::opcode0x9e;
    opcodes[0x9f] = &Processor::opcode0x9f;
    opcodes[0xa0] = &Processor::opcode0xa0;
    opcodes[0xa1] = &Processor::opcode0xa1;
    opcodes[0xa2] = &Processor::opcode0xa2;
    opcodes[0xa3] = &Processor::opcode0xa3;
    opcodes[0xa4] = &Processor::opcode0xa4;
    opcodes[0xa5] = &Processor::opcode0xa5;
    opcodes[0xa6] = &Processor::opcode0xa6;
    opcodes[0xa7] = &Processor::opcode0xa7;
    opcodes[0xa8] = &Processor::opcode0xa8;
    opcodes[0xa9] = &Processor::opcode0xa9;
    opcodes[0xaa] = &Processor::opcode0xaa;
    opcodes[0xab] = &Processor::opcode0xab;
    opcodes[0xac] = &Processor::opcode0xac;
    opcodes[0xad] = &Processor::opcode0xad;
    opcodes[0xae] = &Processor::opcode0xae;
    opcodes[0xaf] = &Processor::opcode0xaf;
    opcodes[0xb0] = &Processor::opcode0xb0;
    opcodes[0xb1] = &Processor::opcode0xb1;
    opcodes[0xb2] = &Processor::opcode0xb2;
    opcodes[0xb3] = &Processor::opcode0xb3;
    opcodes[0xb4] = &Processor::opcode0xb4;
    opcodes[0xb5] = &Processor::opcode0xb5;
    opcodes[0xb6] = &Processor::opcode0xb6;
    opcodes[0xb7] = &Processor::opcode0xb7;
    opcodes[0xb8] = &Processor::opcode0xb8;
    opcodes[0xb9] = &Processor::opcode0xb9;
    opcodes[0xba] = &Processor::opcode0xba;
    opcodes[0xbb] = &Processor::opcode0xbb;
    opcodes[0xbc] = &Processor::opcode0xbc;
    opcodes[0xbd] = &Processor::opcode0xbd;
    opcodes[0xbe] = &Processor::opcode0xbe;
    opcodes[0xbf] = &Processor::opcode0xbf;
    opcodes[0xc0] = &Processor::opcode0xc0;
    opcodes[0xc1] = &Processor::opcode0xc1;
    //opcodes[0xc2] = &Processor::opcode0xc2;
    //opcodes[0xc3] = &Processor::opcode0xc3;
    //opcodes[0xc4] = &Processor::opcode0xc4;
    opcodes[0xc5] = &Processor::opcode0xc5;
    opcodes[0xc6] = &Processor::opcode0xc6;
    //opcodes[0xc7] = &Processor::opcode0xc7;
    opcodes[0xc8] = &Processor::opcode0xc8;
    opcodes[0xc9] = &Processor::opcode0xc9;
    //opcodes[0xca] = &Processor::opcode0xca;
    //opcodes[0xcb] = &Processor::opcode0xcb;
    //opcodes[0xcc] = &Processor::opcode0xcc;
    opcodes[0xcd] = &Processor::opcode0xcd;
    opcodes[0xce] = &Processor::opcode0xce;
    //opcodes[0xcf] = &Processor::opcode0xcf;
    opcodes[0xd0] = &Processor::opcode0xd0;
    opcodes[0xd1] = &Processor::opcode0xd1;
    //opcodes[0xd2] = &Processor::opcode0xd2;
    //opcodes[0xd4] = &Processor::opcode0xd4;
    opcodes[0xd5] = &Processor::opcode0xd5;
    //opcodes[0xd6] = &Processor::opcode0xd6;
    //opcodes[0xd7] = &Processor::opcode0xd7;
    opcodes[0xd8] = &Processor::opcode0xd8;
    //opcodes[0xd9] = &Processor::opcode0xd9;
    //opcodes[0xda] = &Processor::opcode0xda;
    //opcodes[0xdc] = &Processor::opcode0xdc;
    //opcodes[0xde] = &Processor::opcode0xde;
    //opcodes[0xdf] = &Processor::opcode0xdf;
    opcodes[0xe0] = &Processor::opcode0xe0;
    opcodes[0xe1] = &Processor::opcode0xe1;
    opcodes[0xe2] = &Processor::opcode0xe2;
    opcodes[0xe5] = &Processor::opcode0xe5;
    opcodes[0xe6] = &Processor::opcode0xe6;
    //opcodes[0xe7] = &Processor::opcode0xe7;
    opcodes[0xe8] = &Processor::opcode0xe8;
    //opcodes[0xe9] = &Processor::opcode0xe9;
    opcodes[0xea] = &Processor::opcode0xea;
    opcodes[0xee] = &Processor::opcode0xee;
    //opcodes[0xef] = &Processor::opcode0xef;
    opcodes[0xf0] = &Processor::opcode0xf0;
    opcodes[0xf1] = &Processor::opcode0xf1;
    opcodes[0xf2] = &Processor::opcode0xf2;
    //opcodes[0xf3] = &Processor::opcode0xf3;
    opcodes[0xf5] = &Processor::opcode0xf5;
    opcodes[0xf6] = &Processor::opcode0xf6;
    //opcodes[0xf7] = &Processor::opcode0xf7;
    opcodes[0xf8] = &Processor::opcode0xf8;
    opcodes[0xf9] = &Processor::opcode0xf9;
    opcodes[0xfa] = &Processor::opcode0xfa;
    //opcodes[0xfb] = &Processor::opcode0xfb;
    opcodes[0xfe] = &Processor::opcode0xfe;

    cb_opcodes[0x00] = &Processor::cb_opcode0x00;
    cb_opcodes[0x01] = &Processor::cb_opcode0x01;
    cb_opcodes[0x02] = &Processor::cb_opcode0x02;
    cb_opcodes[0x03] = &Processor::cb_opcode0x03;
    cb_opcodes[0x04] = &Processor::cb_opcode0x04;
    cb_opcodes[0x05] = &Processor::cb_opcode0x05;
    cb_opcodes[0x06] = &Processor::cb_opcode0x06;
    cb_opcodes[0x07] = &Processor::cb_opcode0x07;
    cb_opcodes[0x08] = &Processor::cb_opcode0x08;
    cb_opcodes[0x09] = &Processor::cb_opcode0x09;
    cb_opcodes[0x0a] = &Processor::cb_opcode0x0a;
    cb_opcodes[0x0b] = &Processor::cb_opcode0x0b;
    cb_opcodes[0x0c] = &Processor::cb_opcode0x0c;
    cb_opcodes[0x0d] = &Processor::cb_opcode0x0d;
    cb_opcodes[0x0e] = &Processor::cb_opcode0x0e;
    cb_opcodes[0x0f] = &Processor::cb_opcode0x0f;
    cb_opcodes[0x10] = &Processor::cb_opcode0x10;
    cb_opcodes[0x11] = &Processor::cb_opcode0x11;
    cb_opcodes[0x12] = &Processor::cb_opcode0x12;
    cb_opcodes[0x13] = &Processor::cb_opcode0x13;
    cb_opcodes[0x14] = &Processor::cb_opcode0x14;
    cb_opcodes[0x15] = &Processor::cb_opcode0x15;
    cb_opcodes[0x16] = &Processor::cb_opcode0x16;
    cb_opcodes[0x17] = &Processor::cb_opcode0x17;
    cb_opcodes[0x18] = &Processor::cb_opcode0x18;
    cb_opcodes[0x19] = &Processor::cb_opcode0x19;
    cb_opcodes[0x1a] = &Processor::cb_opcode0x1a;
    cb_opcodes[0x1b] = &Processor::cb_opcode0x1b;
    cb_opcodes[0x1c] = &Processor::cb_opcode0x1c;
    cb_opcodes[0x1d] = &Processor::cb_opcode0x1d;
    cb_opcodes[0x1e] = &Processor::cb_opcode0x1e;
    cb_opcodes[0x1f] = &Processor::cb_opcode0x1f;
    cb_opcodes[0x20] = &Processor::cb_opcode0x20;
    cb_opcodes[0x21] = &Processor::cb_opcode0x21;
    cb_opcodes[0x22] = &Processor::cb_opcode0x22;
    cb_opcodes[0x23] = &Processor::cb_opcode0x23;
    cb_opcodes[0x24] = &Processor::cb_opcode0x24;
    cb_opcodes[0x25] = &Processor::cb_opcode0x25;
    cb_opcodes[0x26] = &Processor::cb_opcode0x26;
    cb_opcodes[0x27] = &Processor::cb_opcode0x27;
    cb_opcodes[0x28] = &Processor::cb_opcode0x28;
    cb_opcodes[0x29] = &Processor::cb_opcode0x29;
    cb_opcodes[0x2a] = &Processor::cb_opcode0x2a;
    cb_opcodes[0x2b] = &Processor::cb_opcode0x2b;
    cb_opcodes[0x2c] = &Processor::cb_opcode0x2c;
    cb_opcodes[0x2d] = &Processor::cb_opcode0x2d;
    cb_opcodes[0x2e] = &Processor::cb_opcode0x2e;
    cb_opcodes[0x2f] = &Processor::cb_opcode0x2f;
    cb_opcodes[0x30] = &Processor::cb_opcode0x30;
    cb_opcodes[0x31] = &Processor::cb_opcode0x31;
    cb_opcodes[0x32] = &Processor::cb_opcode0x32;
    cb_opcodes[0x33] = &Processor::cb_opcode0x33;
    cb_opcodes[0x34] = &Processor::cb_opcode0x34;
    cb_opcodes[0x35] = &Processor::cb_opcode0x35;
    cb_opcodes[0x36] = &Processor::cb_opcode0x36;
    cb_opcodes[0x37] = &Processor::cb_opcode0x37;
    cb_opcodes[0x38] = &Processor::cb_opcode0x38;
    cb_opcodes[0x39] = &Processor::cb_opcode0x39;
    cb_opcodes[0x3a] = &Processor::cb_opcode0x3a;
    cb_opcodes[0x3b] = &Processor::cb_opcode0x3b;
    cb_opcodes[0x3c] = &Processor::cb_opcode0x3c;
    cb_opcodes[0x3d] = &Processor::cb_opcode0x3d;
    cb_opcodes[0x3e] = &Processor::cb_opcode0x3e;
    cb_opcodes[0x3f] = &Processor::cb_opcode0x3f;
    cb_opcodes[0x40] = &Processor::cb_opcode0x40;
    cb_opcodes[0x41] = &Processor::cb_opcode0x41;
    cb_opcodes[0x42] = &Processor::cb_opcode0x42;
    cb_opcodes[0x43] = &Processor::cb_opcode0x43;
    cb_opcodes[0x44] = &Processor::cb_opcode0x44;
    cb_opcodes[0x45] = &Processor::cb_opcode0x45;
    cb_opcodes[0x46] = &Processor::cb_opcode0x46;
    cb_opcodes[0x47] = &Processor::cb_opcode0x47;
    cb_opcodes[0x48] = &Processor::cb_opcode0x48;
    cb_opcodes[0x49] = &Processor::cb_opcode0x49;
    cb_opcodes[0x4a] = &Processor::cb_opcode0x4a;
    cb_opcodes[0x4b] = &Processor::cb_opcode0x4b;
    cb_opcodes[0x4c] = &Processor::cb_opcode0x4c;
    cb_opcodes[0x4d] = &Processor::cb_opcode0x4d;
    cb_opcodes[0x4e] = &Processor::cb_opcode0x4e;
    cb_opcodes[0x4f] = &Processor::cb_opcode0x4f;
    cb_opcodes[0x50] = &Processor::cb_opcode0x50;
    cb_opcodes[0x51] = &Processor::cb_opcode0x51;
    cb_opcodes[0x52] = &Processor::cb_opcode0x52;
    cb_opcodes[0x53] = &Processor::cb_opcode0x53;
    cb_opcodes[0x54] = &Processor::cb_opcode0x54;
    cb_opcodes[0x55] = &Processor::cb_opcode0x55;
    cb_opcodes[0x56] = &Processor::cb_opcode0x56;
    cb_opcodes[0x57] = &Processor::cb_opcode0x57;
    cb_opcodes[0x58] = &Processor::cb_opcode0x58;
    cb_opcodes[0x59] = &Processor::cb_opcode0x59;
    cb_opcodes[0x5a] = &Processor::cb_opcode0x5a;
    cb_opcodes[0x5b] = &Processor::cb_opcode0x5b;
    cb_opcodes[0x5c] = &Processor::cb_opcode0x5c;
    cb_opcodes[0x5d] = &Processor::cb_opcode0x5d;
    cb_opcodes[0x5e] = &Processor::cb_opcode0x5e;
    cb_opcodes[0x5f] = &Processor::cb_opcode0x5f;
    cb_opcodes[0x60] = &Processor::cb_opcode0x60;
    cb_opcodes[0x61] = &Processor::cb_opcode0x61;
    cb_opcodes[0x62] = &Processor::cb_opcode0x62;
    cb_opcodes[0x63] = &Processor::cb_opcode0x63;
    cb_opcodes[0x64] = &Processor::cb_opcode0x64;
    cb_opcodes[0x65] = &Processor::cb_opcode0x65;
    cb_opcodes[0x66] = &Processor::cb_opcode0x66;
    cb_opcodes[0x67] = &Processor::cb_opcode0x67;
    cb_opcodes[0x68] = &Processor::cb_opcode0x68;
    cb_opcodes[0x69] = &Processor::cb_opcode0x69;
    cb_opcodes[0x6a] = &Processor::cb_opcode0x6a;
    cb_opcodes[0x6b] = &Processor::cb_opcode0x6b;
    cb_opcodes[0x6c] = &Processor::cb_opcode0x6c;
    cb_opcodes[0x6d] = &Processor::cb_opcode0x6d;
    cb_opcodes[0x6e] = &Processor::cb_opcode0x6e;
    cb_opcodes[0x6f] = &Processor::cb_opcode0x6f;
    cb_opcodes[0x70] = &Processor::cb_opcode0x70;
    cb_opcodes[0x71] = &Processor::cb_opcode0x71;
    cb_opcodes[0x72] = &Processor::cb_opcode0x72;
    cb_opcodes[0x73] = &Processor::cb_opcode0x73;
    cb_opcodes[0x74] = &Processor::cb_opcode0x74;
    cb_opcodes[0x75] = &Processor::cb_opcode0x75;
    cb_opcodes[0x76] = &Processor::cb_opcode0x76;
    cb_opcodes[0x77] = &Processor::cb_opcode0x77;
    cb_opcodes[0x78] = &Processor::cb_opcode0x78;
    cb_opcodes[0x79] = &Processor::cb_opcode0x79;
    cb_opcodes[0x7a] = &Processor::cb_opcode0x7a;
    cb_opcodes[0x7b] = &Processor::cb_opcode0x7b;
    cb_opcodes[0x7c] = &Processor::cb_opcode0x7c;
    cb_opcodes[0x7d] = &Processor::cb_opcode0x7d;
    cb_opcodes[0x7e] = &Processor::cb_opcode0x7e;
    cb_opcodes[0x7f] = &Processor::cb_opcode0x7f;
    cb_opcodes[0x80] = &Processor::cb_opcode0x80;
    cb_opcodes[0x81] = &Processor::cb_opcode0x81;
    cb_opcodes[0x82] = &Processor::cb_opcode0x82;
    cb_opcodes[0x83] = &Processor::cb_opcode0x83;
    cb_opcodes[0x84] = &Processor::cb_opcode0x84;
    cb_opcodes[0x85] = &Processor::cb_opcode0x85;
    cb_opcodes[0x86] = &Processor::cb_opcode0x86;
    cb_opcodes[0x87] = &Processor::cb_opcode0x87;
    cb_opcodes[0x88] = &Processor::cb_opcode0x88;
    cb_opcodes[0x89] = &Processor::cb_opcode0x89;
    cb_opcodes[0x8a] = &Processor::cb_opcode0x8a;
    cb_opcodes[0x8b] = &Processor::cb_opcode0x8b;
    cb_opcodes[0x8c] = &Processor::cb_opcode0x8c;
    cb_opcodes[0x8d] = &Processor::cb_opcode0x8d;
    cb_opcodes[0x8e] = &Processor::cb_opcode0x8e;
    cb_opcodes[0x8f] = &Processor::cb_opcode0x8f;
    cb_opcodes[0x90] = &Processor::cb_opcode0x90;
    cb_opcodes[0x91] = &Processor::cb_opcode0x91;
    cb_opcodes[0x92] = &Processor::cb_opcode0x92;
    cb_opcodes[0x93] = &Processor::cb_opcode0x93;
    cb_opcodes[0x94] = &Processor::cb_opcode0x94;
    cb_opcodes[0x95] = &Processor::cb_opcode0x95;
    cb_opcodes[0x96] = &Processor::cb_opcode0x96;
    cb_opcodes[0x97] = &Processor::cb_opcode0x97;
    cb_opcodes[0x98] = &Processor::cb_opcode0x98;
    cb_opcodes[0x99] = &Processor::cb_opcode0x99;
    cb_opcodes[0x9a] = &Processor::cb_opcode0x9a;
    cb_opcodes[0x9b] = &Processor::cb_opcode0x9b;
    cb_opcodes[0x9c] = &Processor::cb_opcode0x9c;
    cb_opcodes[0x9d] = &Processor::cb_opcode0x9d;
    cb_opcodes[0x9e] = &Processor::cb_opcode0x9e;
    cb_opcodes[0x9f] = &Processor::cb_opcode0x9f;
    cb_opcodes[0xa0] = &Processor::cb_opcode0xa0;
    cb_opcodes[0xa1] = &Processor::cb_opcode0xa1;
    cb_opcodes[0xa2] = &Processor::cb_opcode0xa2;
    cb_opcodes[0xa3] = &Processor::cb_opcode0xa3;
    cb_opcodes[0xa4] = &Processor::cb_opcode0xa4;
    cb_opcodes[0xa5] = &Processor::cb_opcode0xa5;
    cb_opcodes[0xa6] = &Processor::cb_opcode0xa6;
    cb_opcodes[0xa7] = &Processor::cb_opcode0xa7;
    cb_opcodes[0xa8] = &Processor::cb_opcode0xa8;
    cb_opcodes[0xa9] = &Processor::cb_opcode0xa9;
    cb_opcodes[0xaa] = &Processor::cb_opcode0xaa;
    cb_opcodes[0xab] = &Processor::cb_opcode0xab;
    cb_opcodes[0xac] = &Processor::cb_opcode0xac;
    cb_opcodes[0xad] = &Processor::cb_opcode0xad;
    cb_opcodes[0xae] = &Processor::cb_opcode0xae;
    cb_opcodes[0xaf] = &Processor::cb_opcode0xaf;
    cb_opcodes[0xb0] = &Processor::cb_opcode0xb0;
    cb_opcodes[0xb1] = &Processor::cb_opcode0xb1;
    cb_opcodes[0xb2] = &Processor::cb_opcode0xb2;
    cb_opcodes[0xb3] = &Processor::cb_opcode0xb3;
    cb_opcodes[0xb4] = &Processor::cb_opcode0xb4;
    cb_opcodes[0xb5] = &Processor::cb_opcode0xb5;
    cb_opcodes[0xb6] = &Processor::cb_opcode0xb6;
    cb_opcodes[0xb7] = &Processor::cb_opcode0xb7;
    cb_opcodes[0xb8] = &Processor::cb_opcode0xb8;
    cb_opcodes[0xb9] = &Processor::cb_opcode0xb9;
    cb_opcodes[0xba] = &Processor::cb_opcode0xba;
    cb_opcodes[0xbb] = &Processor::cb_opcode0xbb;
    cb_opcodes[0xbc] = &Processor::cb_opcode0xbc;
    cb_opcodes[0xbd] = &Processor::cb_opcode0xbd;
    cb_opcodes[0xbe] = &Processor::cb_opcode0xbe;
    cb_opcodes[0xbf] = &Processor::cb_opcode0xbf;
    cb_opcodes[0xc0] = &Processor::cb_opcode0xc0;
    cb_opcodes[0xc1] = &Processor::cb_opcode0xc1;
    cb_opcodes[0xc2] = &Processor::cb_opcode0xc2;
    cb_opcodes[0xc3] = &Processor::cb_opcode0xc3;
    cb_opcodes[0xc4] = &Processor::cb_opcode0xc4;
    cb_opcodes[0xc5] = &Processor::cb_opcode0xc5;
    cb_opcodes[0xc6] = &Processor::cb_opcode0xc6;
    cb_opcodes[0xc7] = &Processor::cb_opcode0xc7;
    cb_opcodes[0xc8] = &Processor::cb_opcode0xc8;
    cb_opcodes[0xc9] = &Processor::cb_opcode0xc9;
    cb_opcodes[0xca] = &Processor::cb_opcode0xca;
    cb_opcodes[0xcb] = &Processor::cb_opcode0xcb;
    cb_opcodes[0xcc] = &Processor::cb_opcode0xcc;
    cb_opcodes[0xcd] = &Processor::cb_opcode0xcd;
    cb_opcodes[0xce] = &Processor::cb_opcode0xce;
    cb_opcodes[0xcf] = &Processor::cb_opcode0xcf;
    cb_opcodes[0xd0] = &Processor::cb_opcode0xd0;
    cb_opcodes[0xd1] = &Processor::cb_opcode0xd1;
    cb_opcodes[0xd2] = &Processor::cb_opcode0xd2;
    cb_opcodes[0xd3] = &Processor::cb_opcode0xd3;
    cb_opcodes[0xd4] = &Processor::cb_opcode0xd4;
    cb_opcodes[0xd5] = &Processor::cb_opcode0xd5;
    cb_opcodes[0xd6] = &Processor::cb_opcode0xd6;
    cb_opcodes[0xd7] = &Processor::cb_opcode0xd7;
    cb_opcodes[0xd8] = &Processor::cb_opcode0xd8;
    cb_opcodes[0xd9] = &Processor::cb_opcode0xd9;
    cb_opcodes[0xda] = &Processor::cb_opcode0xda;
    cb_opcodes[0xdb] = &Processor::cb_opcode0xdb;
    cb_opcodes[0xdc] = &Processor::cb_opcode0xdc;
    cb_opcodes[0xdd] = &Processor::cb_opcode0xdd;
    cb_opcodes[0xde] = &Processor::cb_opcode0xde;
    cb_opcodes[0xdf] = &Processor::cb_opcode0xdf;
    cb_opcodes[0xe0] = &Processor::cb_opcode0xe0;
    cb_opcodes[0xe1] = &Processor::cb_opcode0xe1;
    cb_opcodes[0xe2] = &Processor::cb_opcode0xe2;
    cb_opcodes[0xe3] = &Processor::cb_opcode0xe3;
    cb_opcodes[0xe4] = &Processor::cb_opcode0xe4;
    cb_opcodes[0xe5] = &Processor::cb_opcode0xe5;
    cb_opcodes[0xe6] = &Processor::cb_opcode0xe6;
    cb_opcodes[0xe7] = &Processor::cb_opcode0xe7;
    cb_opcodes[0xe8] = &Processor::cb_opcode0xe8;
    cb_opcodes[0xe9] = &Processor::cb_opcode0xe9;
    cb_opcodes[0xea] = &Processor::cb_opcode0xea;
    cb_opcodes[0xeb] = &Processor::cb_opcode0xeb;
    cb_opcodes[0xec] = &Processor::cb_opcode0xec;
    cb_opcodes[0xed] = &Processor::cb_opcode0xed;
    cb_opcodes[0xee] = &Processor::cb_opcode0xee;
    cb_opcodes[0xef] = &Processor::cb_opcode0xef;
    cb_opcodes[0xf0] = &Processor::cb_opcode0xf0;
    cb_opcodes[0xf1] = &Processor::cb_opcode0xf1;
    cb_opcodes[0xf2] = &Processor::cb_opcode0xf2;
    cb_opcodes[0xf3] = &Processor::cb_opcode0xf3;
    cb_opcodes[0xf4] = &Processor::cb_opcode0xf4;
    cb_opcodes[0xf5] = &Processor::cb_opcode0xf5;
    cb_opcodes[0xf6] = &Processor::cb_opcode0xf6;
    cb_opcodes[0xf7] = &Processor::cb_opcode0xf7;
    cb_opcodes[0xf8] = &Processor::cb_opcode0xf8;
    cb_opcodes[0xf9] = &Processor::cb_opcode0xf9;
    cb_opcodes[0xfa] = &Processor::cb_opcode0xfa;
    cb_opcodes[0xfb] = &Processor::cb_opcode0xfb;
    cb_opcodes[0xfc] = &Processor::cb_opcode0xfc;
    cb_opcodes[0xfd] = &Processor::cb_opcode0xfd;
    cb_opcodes[0xfe] = &Processor::cb_opcode0xfe;
    cb_opcodes[0xff] = &Processor::cb_opcode0xff;

}
