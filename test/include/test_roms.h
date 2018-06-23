#include "definitions.h"
#include <vector>

namespace test {

    /*  Expected output
        AF: 1100
        BC: 23f3
        DE: ad99
        HL: bbff
        SP: fffe (-2)
        PC: 0000
    */
    std::vector<u8> ld_immediate_8bit = {
        0x3e, 0x11,
        0x06, 0x23,
        0x0e, 0xf3,
        0x16, 0xad,
        0x1e, 0x99,
        0x26, 0xbb,
        0x2e, 0xff
    };

    /*  Expected output
        AF: ----
        BC: 32ff
        DE: 003d
        HL: 3234
        SP: 1f00 
        PC: 0000
    */
    std::vector<u8> ld_immediate_16bit = {
        0x01, 0xff, 0x32,
        0x11, 0x3d, 0x00,
        0x21, 0x34, 0x32,
        0x31, 0x00, 0x1f,
    };

    /*  Expected output:
        AF: ae00
        BC: aeae
        DE: aeae
        HL: aeae
        SP: ----
        PC: 0000
    */  
    std::vector<u8> ld_register_8bit = {
        0x3e, 0xae,
        0x47, 0x48,
        0x51, 0x5a,
        0x63, 0x6c,
    };
        
    /*  Expected output:
        AF: be00
        BC: bebe
        DE: bebe
        HL: ffff
        SP: 1efa
        PC: 0000
    */
    std::vector<u8> ld_address = {
        0x21, 0x10, 0x80,       // LD HL, 8010
        0x36, 0xbe,
        0x7e,
        0x46,
        0x4e,
        0x56,
        0x5e,
        0x77
    };

    // AF: 14f0
    // DE: 0e34
    // BC: aabb
    // SP: ff00
    std::vector<u8> push_pop = {
        0x31, 0x00, 0xff,   // LD SP, ff00
        0x21, 0xbb, 0xaa,   // LD HL, aabb
        0xe5,               // PUSH HL
        0x21, 0x34, 0x0e,   // LD HL, 0e34
        0xe5,               // PUSH HL
        0x21, 0xf3, 0x14,   // LD HL, 14f3
        0xe5,               // PUSH HL
        0xf1,               // POP AF
        0xd1,               // POP DE
        0xc1                // POP BC
    };

    std::vector<u8> add = {

    };

    std::vector<u8> sub = {

    };

    std::vector<u8> and_op = {

    };

    std::vector<u8> or_op = {

    };

    std::vector<u8> xor_op = {

    };

    std::vector<u8> cp = {

    };

    std::vector<u8> cpl = {

    };

    std::vector<u8> swap = {

    };

    /*  Expected output:
        AF: d200
    */
    std::vector<u8> rotate_right_carry = {
        0x37,       // SCF, CF <- 1
        0x3e, 0xa4, // LD A, 10100100
        0x1f        // RRA, A <- 11010010, CF <- 0
    };

    /*  Expected output:
        AF: --10
        BC: d2--
    */
    std::vector<u8> rotate_right = {
        0x37,       // SCF
        0x3f,       // CCF, CF <- 0
        0x06, 0xa5, // LD B, 10100101
        0xcb, 0x08  // RRC B, B <- 110100101, CF <- 1
    };

    /*  Expected output:
        AF: 6510
        HL: ffff
    */
    std::vector<u8> rotate_right_carry_mem = {
        0x37,               // SCF
        0x3f,               // CCF, CF <- 0
        0x21, 0xff, 0xff,   // LD HL, 0xffff
        0x36, 0xcb,         // LD (HL), 11001011
        0xcb, 0x1e,         // RR (HL), (HL) <- 01100101, CF <- 1
        0x7e                // LD A, (HL)
    };

    /*  Expected output:
        AF: 2500
        HL: 1000
    */
    std::vector<u8> rotate_right_mem = {
        0x37,               // SCF
        0x3f,               // CCF, CF <- 0
        0x21, 0x00, 0x10,   // LD HL, 0x1000
        0x36, 0x4a,         // LD (HL), 01001010
        0xcb, 0x0e,         // RRC (HL), (HL) <- 00100101, CF <- 0
        0x7e                // LD A, (HL)
    };

    /*  Expected output:
        AF: 4810
    */
    std::vector<u8> rotate_left_carry = {
        0x37,       // SCF
        0x3f,       // CCF, CF <- 0
        0x3e, 0xa4, // LD A, 10100100
        0x17        // RLA, A <- 01001000, CF <- 1
    };

    /*  Expected output:
        AF: 4910
    */
    std::vector<u8> rotate_left = {
        0x37,       // SCF
        0x3f,       // CCF, CF <- 0
        0x3e, 0xa4, // LD A, 10100100
        0x07        // RLCA , A <- 01001001, CF <- 1
    };

    /*  Expected output:
        AF: 9610
        HL: ffff
    */
    std::vector<u8> rotate_left_carry_mem = {
        0x37,
        0x3f,               // CCF, CF <- 0
        0x21, 0xff, 0xff,   // LD HL, 0xffff
        0x36, 0xcb,         // LD (HL), 11001011
        0xcb, 0x16,         // RL (HL), (HL) <- 100100110, CF <- 1
        0x7e                // LD A, (HL)
    };

    /*  Expected output:
        AF: 9400
        HL: 1000
    */
    std::vector<u8> rotate_left_mem = {
        0x37,               // SCF
        0x21, 0x00, 0x10,   // LD HL, 0x1000
        0x36, 0x4a,         // LD (HL), 01001010
        0xcb, 0x06,         // RLC (HL), (HL) <- 10010100, CF <- 0
        0x7e                // LD A, (HL)
    };

    // AF: 7c00
    std::vector<u8> shift_left = {
        0x37,       // SCF, CF <- 1
        0x3e, 0x3e, // LD A, 00111110
        0xcb, 0x27, // SLA A, A <- 01111100, CF <- 0
    };

    // AF: 7c10
    std::vector<u8> shift_left_mem = {
        0x37,               // SCF
        0x3f,               // CCF, CF <- 0
        0x21, 0xff, 0xff,   // LD HL, 0xffff
        0x36, 0xbe,         // LD (HL), 10111110
        0xcb, 0x26,         // SLA (HL), (HL) <- 01111100, CF <- 1
        0x7e                // LD A, (HL)
    };

    // AF: df00
    std::vector<u8> shift_right_arithmetic = {
        0x37,       // SCF, CF <- 1
        0x3e, 0xbe, // LD A, 10111110
        0xcb, 0x2f, // SRA A, A <- 11011111, CF <- 0
    };

    // AF: 0a00
    std::vector<u8> shift_right_arithmetic_mem = {
        0x37,               // SCF
        0x3f,               // CCF, CF <- 0
        0x21, 0xff, 0xff,   // LD HL, 0xffff
        0x36, 0x15,         // LD (HL), 00010101
        0xcb, 0x2e,         // SRA (HL), (HL) <- 00001010, CF <- 1
        0x7e                // LD A, (HL)
    };

    // AF: 5f00
    std::vector<u8> shift_right_logical = {
        0x37,       // SCF, CF <- 1
        0x3e, 0xbe, // LD A, 10111110
        0xcb, 0x3f, // SRL A, A <- 01011111, CF <- 0
    };

    // AF: 0090
    std::vector<u8> shift_right_logical_mem = {
        0x37,               // SCF
        0x3f,               // CCF, CF <- 0
        0x21, 0xff, 0xff,   // LD HL, 0xffff
        0x36, 0x01,         // LD (HL), 00000001
        0xcb, 0x3e,         // SRL (HL), (HL) <- 00000000, CF <- 1, Z <- 1
        0x7e                // LD A, (HL)
    };

    // AF: 2030
    std::vector<u8> bit_set = {
        0x37,           // SCF, CF <- 1
        0x3e, 0x00,     // LD A, 0
        0xcb, 0xef,     // SET 5, A
        0xcb, 0x6f      // BIT 5, A, Z <- 0, HCF <- 1
    };

    // AF: fea0
    std::vector<u8> bit_reset = {
        0x37,           // SCF
        0x3f,           // CCF
        0x3e, 0xff,     // LD A, ff
        0xcb, 0x87,     // RES 0, A <- fe
        0xcb, 0x47     // BIT 0, A, Z <- 1, HCF <- 1
    };

    // AF: 0x1530
    std::vector<u8> bit_mem_set = {
        0x37,               // SCF, CF <- 1
        0x21, 0xff, 0xff,   // LD HL, 0xffff
        0x36, 0x15,         // LD (HL), 00010101
        0xcb, 0x56,         // BIT 2, (HL), Z <- 0, HCF <- 1
        0x7e                // LD A, (HL)
    };

    // AF: 15a0
    std::vector<u8> bit_mem_reset = {
        0x37,               // SCF, CF <- 1
        0x3f,               // CCF, CF <- 0
        0x21, 0xff, 0xff,   // LD HL, 0xffff
        0x36, 0x15,         // LD (HL), 00010101
        0xcb, 0x7e,         // BIT 7, (HL), Z <- 1, HCF <- 1
        0x7e                // LD A, (HL)
    };
    
    // PC: 0x2fc1
    std::vector<u8> jp = {
        0xc3, 0xc1, 0x2f    // JP 0x2fc1
    };

    // PC: 0xffff
    std::vector<u8> jp_reg = {
        0x21, 0xff, 0xff,   // LD HL, ffff
        0xe9                // JP (HL)
    };

    // PC: 0006 
    std::vector<u8> jr_negative = {
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00,         // NOP (x8)
        0x18, 0xfc          // JR fd (-4)
    };

    // PC: 0016
    std::vector<u8> jr_positive = {
        0x00, 0x00,
        0x00, 0x00,
        0x18, 0x10          // JR 0x10 (16)
    };

    // PC: 0004
    std::vector<u8> jr_zero = {
        0x00, 0x00,          
        0x18, 0x00          // JR 00
    };

    std::vector<u8> call = {
        
    };

}
