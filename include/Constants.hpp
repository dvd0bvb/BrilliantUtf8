#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
#include <array>
#include <iterator>
#include <ranges>

namespace brilliant {
    namespace utf8 {

        constexpr char8_t ONE_BYTE_PREFIX =     0b00000000; 
        constexpr char8_t TWO_BYTE_PREFIX =     0b11000000;
        constexpr char8_t THREE_BYTE_PREFIX =   0b11100000;
        constexpr char8_t FOUR_BYTE_PREFIX =    0b11110000;
        constexpr char8_t CONTINUATION_PREFIX = 0b10000000;

        constexpr std::array<char8_t, 4> PREFIXES{
            ONE_BYTE_PREFIX,
            TWO_BYTE_PREFIX,
            THREE_BYTE_PREFIX,
            FOUR_BYTE_PREFIX
        };

        constexpr char8_t ONE_BYTE_MASK =     0b10000000;
        constexpr char8_t TWO_BYTE_MASK =     0b11100000;
        constexpr char8_t THREE_BYTE_MASK =   0b11110000;
        constexpr char8_t FOUR_BYTE_MASK =    0b11111000;
        constexpr char8_t CONTINUATION_MASK = 0b11000000;

        constexpr std::array<char8_t, 4> MASKS{
            ONE_BYTE_MASK,
            TWO_BYTE_MASK,
            THREE_BYTE_MASK,
            FOUR_BYTE_MASK
        };
    } // namespace utf8 
} // namespace brilliant 