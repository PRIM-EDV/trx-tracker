/* Copyright (c) 2022, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef RAND_HPP
#define RAND_HPP

#define LFSR8_SEED      (0xa4)
#define LFSR16_SEED     (0x421f)
#define LFSR32_SEED     (0x0283ba3f)
#define LFSR64_SEED     (0x216a75f98e439d2e)

#include <stdint.h>

namespace random
{
    inline uint8_t lfsr8()
    {
        uint8_t feedback = 0;
        static uint8_t state = LFSR8_SEED;

        feedback = (state >> 0) ^ (state >> 2) ^ (state >> 3) ^ (state >> 4);
        state = (state >> 1) | (feedback << 7);
        return state;
    }

    inline uint16_t lfsr16()
    {
        uint16_t feedback = 0;
        static uint16_t state = LFSR16_SEED;

        feedback = (state >> 0) ^ (state >> 2) ^ (state >> 3) ^ (state >> 5);
        state = (state >> 1) | (feedback << 15);
        return state;
    }

    inline uint32_t lfsr32()
    {
        uint32_t feedback = 0;
        static uint32_t state = LFSR32_SEED;

        feedback = (state >> 0) ^ (state >> 2) ^ (state >> 6) ^ (state >> 7);
        state = (state >> 1) | (feedback << 31);
        return state;
    }

    inline uint64_t lfsr64()
    {
        uint64_t feedback = 0;
        static uint64_t state = LFSR64_SEED;

        feedback = (state >> 0) ^ (state >> 1) ^ (state >> 3) ^ (state >> 4);
        state = (state >> 1) | (feedback << 63);
        return state;
    }
}

#endif