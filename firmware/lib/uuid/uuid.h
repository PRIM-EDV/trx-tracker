/* Copyright (c) 2022, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef UUID_HPP
#define UUID_HPP

#include <stdint.h>
#include "lib/random/random.hpp"

using namespace modm;

namespace uuid {

    namespace
    {
        const char alphabet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        union RandomBytes {
            uint64_t source[2];
            uint8_t bytes[16];
        };

        void puthex(uint8_t pos, uint8_t value, char* buffer){
            buffer[pos] = alphabet[value >> 4];
            buffer[pos + 1] = alphabet[value & 0x0f];
        }

        void puthexs(uint8_t pos, uint8_t count, uint8_t *values, char* buffer)
        {
            for(uint8_t i=0; i< count; i++)
            {
                puthex(pos + (i << 1), values[i], buffer);
            }
        }
    }

    inline void v4(char* buffer)
    {
        RandomBytes rand = {random::lfsr64(), random::lfsr64()};

        rand.bytes[6] = (rand.bytes[6] & 0x0F) | 0x40;
        rand.bytes[8] = (rand.bytes[8] & 0x3F) | 0x80;

        puthexs(0,  4,  &rand.bytes[0], buffer);
        puthexs(9,  2,  &rand.bytes[4], buffer);
        puthexs(14, 2,  &rand.bytes[6], buffer);
        puthexs(19, 2,  &rand.bytes[8], buffer);
        puthexs(24, 6,  &rand.bytes[10], buffer);

        buffer[8] = '-', buffer[13] = '-', buffer[18] = '-', buffer[23] = '-'; buffer[37] = '\0';
    }
}

#endif