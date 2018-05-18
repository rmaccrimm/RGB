#include "definitions.h"
#include <vector>

namespace TEST {

    /*  Each program ends with Jump 0, and needs a break point at 0
        Expected SP values are for this order, since Jump pushes to values to stack (SP -= 2)
    */

    /*  Expected output
        AF: 1100
        BC: 23f3
        DE: ad99
        HL: 00ff
        SP: fffe (-2)
        PC: 0000
    */
    std::vector<u8> ld_immediate_8bit = {
        0x3e, 0x11,
        0x06, 0x23,
        0x0e, 0xf3,
        0x16, 0xad,
        0x1e, 0x99,
        0x26, 0x00,
        0x2e, 0xff,
        0xcd, 0x00, 0x00
    };

    /*  Expected output
        AF: ----
        BC: 32ff
        DE: 003d
        HL: 3234
        SP: 1efe (1f00 - 2)
        PC: 0000
    */
    std::vector<u8> ld_immediate_16bit = {
        0x01, 0xff, 0x32,
        0x11, 0x3d, 0x00,
        0x21, 0x34, 0x32,
        0x31, 0x00, 0x1f,
        0xcd, 0x00, 0x00
    };

    /*  Expected output:
        AF: ae00
        BC: aeae
        DE: aeae
        HL: aeae
        SP: 1efc
        PC: 0000
    */  
    std::vector<u8> ld_register_8bit = {
        0x3e, 0xae,
        0x47, 0x48,
        0x51, 0x5a,
        0x63, 0x6c,
        0xcd, 0x00, 0x00
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
        0x21, 0xff, 0xff,
        0x36, 0xbe,
        0x7e,
        0x46,
        0x4e,
        0x56,
        0x5e,
        0x77,
        0xcd, 0x00, 0x00
    };

    /*  Expected output:
        AF: ----
        BC: ffee
        DE: ddcc
        HL: bbaa
        SP: 1ef8
        PC: 0000
    */
    std::vector<u8> stack = {
        0x01, 0xaa, 0xbb,
        0x11, 0xcc, 0xdd,
        0x21, 0xee, 0xff,
        0xc5,
        0xd5,
        0xe5,
        0xc1,
        0xd1,
        0xe1,
        0xcd, 0x00, 0x00
    };
}