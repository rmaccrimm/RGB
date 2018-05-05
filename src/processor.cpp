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

bool Processor::step()
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
        std::cout << prefix << std::hex << (int)instr << std::endl;
        print_register_values();
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

Processor::Processor(u8 *mem):
    A(), F(), B(), C(), D(), E(), H(), L(),
    AF(&A, &F), BC(&B, &C),	DE(&D, &F), HL(&H, &L), memory(mem)
{
    for (unsigned int i = 0; i < 0x100; i++) {
        opcodes[i] = nullptr;
        cb_opcodes[i] = nullptr;
    }
    // opcodes[0x00] = &Processor::opcode0x00;
    // opcodes[0x01] = &Processor::opcode0x01;
    // opcodes[0x02] = &Processor::opcode0x02;
    // opcodes[0x03] = &Processor::opcode0x03;
    opcodes[0x04] = &Processor::opcode0x04;
    // opcodes[0x05] = &Processor::opcode0x05;
    opcodes[0x06] = &Processor::opcode0x06;
    opcodes[0x07] = &Processor::opcode0x07;
    // opcodes[0x08] = &Processor::opcode0x08;
    // opcodes[0x09] = &Processor::opcode0x09;
    // opcodes[0x0a] = &Processor::opcode0x0a;
    // opcodes[0x0b] = &Processor::opcode0x0b;
    opcodes[0x0c] = &Processor::opcode0x0c;
    // opcodes[0x0d] = &Processor::opcode0x0d;
    opcodes[0x0e] = &Processor::opcode0x0e;
    opcodes[0x0f] = &Processor::opcode0x0f;
    // opcodes[0x10] = &Processor::opcode0x10;
    opcodes[0x11] = &Processor::opcode0x11;
    // opcodes[0x12] = &Processor::opcode0x12;
    // opcodes[0x13] = &Processor::opcode0x13;
    opcodes[0x14] = &Processor::opcode0x14;
    // opcodes[0x15] = &Processor::opcode0x15;
    // opcodes[0x16] = &Processor::opcode0x16;
    opcodes[0x17] = &Processor::opcode0x17;
    // opcodes[0x18] = &Processor::opcode0x18;
    // opcodes[0x19] = &Processor::opcode0x19;
    opcodes[0x1a] = &Processor::opcode0x1a;
    // opcodes[0x1b] = &Processor::opcode0x1b;
    opcodes[0x1c] = &Processor::opcode0x1c;
    // opcodes[0x1d] = &Processor::opcode0x1d;
    // opcodes[0x1e] = &Processor::opcode0x1e;
    opcodes[0x1f] = &Processor::opcode0x1f;
    opcodes[0x20] = &Processor::opcode0x20;
    opcodes[0x21] = &Processor::opcode0x21;
    // opcodes[0x22] = &Processor::opcode0x22;
    // opcodes[0x23] = &Processor::opcode0x23;
    opcodes[0x24] = &Processor::opcode0x24;
    // opcodes[0x25] = &Processor::opcode0x25;
    // opcodes[0x26] = &Processor::opcode0x26;
    // opcodes[0x27] = &Processor::opcode0x27;
    // opcodes[0x28] = &Processor::opcode0x28;
    // opcodes[0x29] = &Processor::opcode0x29;
    // opcodes[0x2a] = &Processor::opcode0x2a;
    // opcodes[0x2b] = &Processor::opcode0x2b;
    opcodes[0x2c] = &Processor::opcode0x2c;
    // opcodes[0x2d] = &Processor::opcode0x2d;
    // opcodes[0x2e] = &Processor::opcode0x2e;
    // opcodes[0x2f] = &Processor::opcode0x2f;
    // opcodes[0x30] = &Processor::opcode0x30;
    opcodes[0x31] = &Processor::opcode0x31;
    opcodes[0x32] = &Processor::opcode0x32;
    // opcodes[0x33] = &Processor::opcode0x33;
    //opcodes[0x34] = &Processor::opcode0x34;
    // opcodes[0x35] = &Processor::opcode0x35;
    // opcodes[0x36] = &Processor::opcode0x36;
    // opcodes[0x37] = &Processor::opcode0x37;
    // opcodes[0x38] = &Processor::opcode0x38;
    // opcodes[0x39] = &Processor::opcode0x39;
    // opcodes[0x3a] = &Processor::opcode0x3a;
    // opcodes[0x3b] = &Processor::opcode0x3b;
    opcodes[0x3c] = &Processor::opcode0x3c;
    // opcodes[0x3d] = &Processor::opcode0x3d;
    opcodes[0x3e] = &Processor::opcode0x3e;
    // opcodes[0x3f] = &Processor::opcode0x3f;
    // opcodes[0x40] = &Processor::opcode0x40;
    // opcodes[0x41] = &Processor::opcode0x41;
    //opcodes[0x42] = &Processor::opcode0x42;
    // opcodes[0x43] = &Processor::opcode0x43;
    // opcodes[0x44] = &Processor::opcode0x44;
    // opcodes[0x45] = &Processor::opcode0x45;
    // opcodes[0x46] = &Processor::opcode0x46;
    // opcodes[0x47] = &Processor::opcode0x47;
    // opcodes[0x48] = &Processor::opcode0x48;
    // opcodes[0x49] = &Processor::opcode0x49;
    // opcodes[0x4a] = &Processor::opcode0x4a;
    // opcodes[0x4b] = &Processor::opcode0x4b;
    // opcodes[0x4c] = &Processor::opcode0x4c;
    // opcodes[0x4d] = &Processor::opcode0x4d;
    // opcodes[0x4e] = &Processor::opcode0x4e;
    opcodes[0x4f] = &Processor::opcode0x4f;
    // opcodes[0x50] = &Processor::opcode0x50;
    // opcodes[0x51] = &Processor::opcode0x51;
    // opcodes[0x52] = &Processor::opcode0x52;
    // opcodes[0x53] = &Processor::opcode0x53;
    // opcodes[0x54] = &Processor::opcode0x54;
    // opcodes[0x55] = &Processor::opcode0x55;
    // opcodes[0x56] = &Processor::opcode0x56;
    // opcodes[0x57] = &Processor::opcode0x57;
    // opcodes[0x58] = &Processor::opcode0x58;
    // opcodes[0x59] = &Processor::opcode0x59;
    // opcodes[0x5a] = &Processor::opcode0x5a;
    // opcodes[0x5b] = &Processor::opcode0x5b;
    // opcodes[0x5c] = &Processor::opcode0x5c;
    // opcodes[0x5d] = &Processor::opcode0x5d;
    // opcodes[0x5e] = &Processor::opcode0x5e;
    // opcodes[0x5f] = &Processor::opcode0x5f;
    // opcodes[0x60] = &Processor::opcode0x60;
    // opcodes[0x61] = &Processor::opcode0x61;
    // opcodes[0x62] = &Processor::opcode0x62;
    // opcodes[0x63] = &Processor::opcode0x63;
    // opcodes[0x64] = &Processor::opcode0x64;
    // opcodes[0x65] = &Processor::opcode0x65;
    // opcodes[0x66] = &Processor::opcode0x66;
    // opcodes[0x67] = &Processor::opcode0x67;
    // opcodes[0x68] = &Processor::opcode0x68;
    // opcodes[0x69] = &Processor::opcode0x69;
    // opcodes[0x6a] = &Processor::opcode0x6a;
    // opcodes[0x6b] = &Processor::opcode0x6b;
    // opcodes[0x6c] = &Processor::opcode0x6c;
    // opcodes[0x6d] = &Processor::opcode0x6d;
    // opcodes[0x6e] = &Processor::opcode0x6e;
    // opcodes[0x6f] = &Processor::opcode0x6f;
    // opcodes[0x70] = &Processor::opcode0x70;
    // opcodes[0x71] = &Processor::opcode0x71;
    // opcodes[0x72] = &Processor::opcode0x72;
    // opcodes[0x73] = &Processor::opcode0x73;
    // opcodes[0x74] = &Processor::opcode0x74;
    // opcodes[0x75] = &Processor::opcode0x75;
    // opcodes[0x76] = &Processor::opcode0x76;
    opcodes[0x77] = &Processor::opcode0x77;
    // opcodes[0x78] = &Processor::opcode0x78;
    // opcodes[0x79] = &Processor::opcode0x79;
    // opcodes[0x7a] = &Processor::opcode0x7a;
    // opcodes[0x7b] = &Processor::opcode0x7b;
    // opcodes[0x7c] = &Processor::opcode0x7c;
    // opcodes[0x7d] = &Processor::opcode0x7d;
    // opcodes[0x7e] = &Processor::opcode0x7e;
    // opcodes[0x7f] = &Processor::opcode0x7f;
    // opcodes[0x80] = &Processor::opcode0x80;
    // opcodes[0x81] = &Processor::opcode0x81;
    // opcodes[0x82] = &Processor::opcode0x82;
    // opcodes[0x83] = &Processor::opcode0x83;
    // opcodes[0x84] = &Processor::opcode0x84;
    // opcodes[0x85] = &Processor::opcode0x85;
    // opcodes[0x86] = &Processor::opcode0x86;
    // opcodes[0x87] = &Processor::opcode0x87;
    // opcodes[0x88] = &Processor::opcode0x88;
    // opcodes[0x89] = &Processor::opcode0x89;
    // opcodes[0x8a] = &Processor::opcode0x8a;
    // opcodes[0x8b] = &Processor::opcode0x8b;
    // opcodes[0x8c] = &Processor::opcode0x8c;
    // opcodes[0x8d] = &Processor::opcode0x8d;
    // opcodes[0x8e] = &Processor::opcode0x8e;
    // opcodes[0x8f] = &Processor::opcode0x8f;
    // opcodes[0x90] = &Processor::opcode0x90;
    // opcodes[0x91] = &Processor::opcode0x91;
    // opcodes[0x92] = &Processor::opcode0x92;
    // opcodes[0x93] = &Processor::opcode0x93;
    // opcodes[0x94] = &Processor::opcode0x94;
    // opcodes[0x95] = &Processor::opcode0x95;
    // opcodes[0x96] = &Processor::opcode0x96;
    // opcodes[0x97] = &Processor::opcode0x97;
    // opcodes[0x98] = &Processor::opcode0x98;
    // opcodes[0x99] = &Processor::opcode0x99;
    // opcodes[0x9a] = &Processor::opcode0x9a;
    // opcodes[0x9b] = &Processor::opcode0x9b;
    // opcodes[0x9c] = &Processor::opcode0x9c;
    // opcodes[0x9d] = &Processor::opcode0x9d;
    // opcodes[0x9e] = &Processor::opcode0x9e;
    // opcodes[0x9f] = &Processor::opcode0x9f;
    // opcodes[0xa0] = &Processor::opcode0xa0;
    // opcodes[0xa1] = &Processor::opcode0xa1;
    // opcodes[0xa2] = &Processor::opcode0xa2;
    // opcodes[0xa3] = &Processor::opcode0xa3;
    // opcodes[0xa4] = &Processor::opcode0xa4;
    // opcodes[0xa5] = &Processor::opcode0xa5;
    // opcodes[0xa6] = &Processor::opcode0xa6;
    // opcodes[0xa7] = &Processor::opcode0xa7;
    // opcodes[0xa8] = &Processor::opcode0xa8;
    // opcodes[0xa9] = &Processor::opcode0xa9;
    // opcodes[0xaa] = &Processor::opcode0xaa;
    // opcodes[0xab] = &Processor::opcode0xab;
    // opcodes[0xac] = &Processor::opcode0xac;
    // opcodes[0xad] = &Processor::opcode0xad;
    // opcodes[0xae] = &Processor::opcode0xae;
    opcodes[0xaf] = &Processor::opcode0xaf;
    // opcodes[0xb0] = &Processor::opcode0xb0;
    // opcodes[0xb1] = &Processor::opcode0xb1;
    // opcodes[0xb2] = &Processor::opcode0xb2;
    // opcodes[0xb3] = &Processor::opcode0xb3;
    // opcodes[0xb4] = &Processor::opcode0xb4;
    // opcodes[0xb5] = &Processor::opcode0xb5;
    // opcodes[0xb6] = &Processor::opcode0xb6;
    // opcodes[0xb7] = &Processor::opcode0xb7;
    // opcodes[0xb8] = &Processor::opcode0xb8;
    // opcodes[0xb9] = &Processor::opcode0xb9;
    // opcodes[0xba] = &Processor::opcode0xba;
    // opcodes[0xbb] = &Processor::opcode0xbb;
    // opcodes[0xbc] = &Processor::opcode0xbc;
    // opcodes[0xbd] = &Processor::opcode0xbd;
    // opcodes[0xbe] = &Processor::opcode0xbe;
    // opcodes[0xbf] = &Processor::opcode0xbf;
    // opcodes[0xc0] = &Processor::opcode0xc0;
    opcodes[0xc1] = &Processor::opcode0xc1;
    // opcodes[0xc2] = &Processor::opcode0xc2;
    // opcodes[0xc3] = &Processor::opcode0xc3;
    // opcodes[0xc4] = &Processor::opcode0xc4;
    opcodes[0xc5] = &Processor::opcode0xc5;
    // opcodes[0xc6] = &Processor::opcode0xc6;
    // opcodes[0xc7] = &Processor::opcode0xc7;
    // opcodes[0xc8] = &Processor::opcode0xc8;
    // opcodes[0xc9] = &Processor::opcode0xc9;
    // opcodes[0xca] = &Processor::opcode0xca;
    // opcodes[0xcb] = &Processor::opcode0xcb;
    // opcodes[0xcc] = &Processor::opcode0xcc;
    opcodes[0xcd] = &Processor::opcode0xcd;
    // opcodes[0xce] = &Processor::opcode0xce;
    // opcodes[0xcf] = &Processor::opcode0xcf;
    // opcodes[0xd0] = &Processor::opcode0xd0;
    opcodes[0xd1] = &Processor::opcode0xd1;
    // opcodes[0xd2] = &Processor::opcode0xd2;
    // opcodes[0xd4] = &Processor::opcode0xd4;
    opcodes[0xd5] = &Processor::opcode0xd5;
    // opcodes[0xd6] = &Processor::opcode0xd6;
    // opcodes[0xd7] = &Processor::opcode0xd7;
    // opcodes[0xd8] = &Processor::opcode0xd8;
    // opcodes[0xd9] = &Processor::opcode0xd9;
    // opcodes[0xda] = &Processor::opcode0xda;
    // opcodes[0xdc] = &Processor::opcode0xdc;
    // opcodes[0xde] = &Processor::opcode0xde;
    // opcodes[0xdf] = &Processor::opcode0xdf;
    opcodes[0xe0] = &Processor::opcode0xe0;
    opcodes[0xe1] = &Processor::opcode0xe1;
    opcodes[0xe2] = &Processor::opcode0xe2;
    opcodes[0xe5] = &Processor::opcode0xe5;
    // opcodes[0xe6] = &Processor::opcode0xe6;
    // opcodes[0xe7] = &Processor::opcode0xe7;
    // opcodes[0xe8] = &Processor::opcode0xe8;
    // opcodes[0xe9] = &Processor::opcode0xe9;
    // opcodes[0xea] = &Processor::opcode0xea;
    // opcodes[0xee] = &Processor::opcode0xee;
    // opcodes[0xef] = &Processor::opcode0xef;
    // opcodes[0xf0] = &Processor::opcode0xf0;
    opcodes[0xf1] = &Processor::opcode0xf1;
    // opcodes[0xf2] = &Processor::opcode0xf2;
    // opcodes[0xf3] = &Processor::opcode0xf3;
    opcodes[0xf5] = &Processor::opcode0xf5;
    // opcodes[0xf6] = &Processor::opcode0xf6;
    // opcodes[0xf7] = &Processor::opcode0xf7;
    // opcodes[0xf8] = &Processor::opcode0xf8;
    // opcodes[0xf9] = &Processor::opcode0xf9;
    // opcodes[0xfa] = &Processor::opcode0xfa;
    // opcodes[0xfb] = &Processor::opcode0xfb;
    // opcodes[0xfe] = &Processor::opcode0xfe;

    cb_opcodes[0x11] = &Processor::cb_opcode0x11;
    cb_opcodes[0x7c] = &Processor::cb_opcode0x7c;
}
