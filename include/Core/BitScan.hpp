#pragma once
#include <cstdint>
#include <array>
#include <Core/Types.hpp>

namespace Chess::Core::BitScan {
    static constexpr std::array<uint8_t, 64> bitScanIndex {
        0, 47, 1, 56, 48, 27, 2, 60,
        57, 49, 41, 37, 28, 16, 3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11, 4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30, 9, 24,
        13, 18, 8, 12, 7, 6, 5, 63
    };
    /**
     * @author Kim Walisch (2012)
     * @return index (0..63) of least significant one bit
     */
    // For search of the least significant bit
    constexpr Chess::Types::Square Forward(Bitboard bitboard) {
        const Bitboard debruijn64 = 0x03f79d71b4cb0a89ULL;
        return static_cast<Chess::Types::Square>(bitScanIndex[((bitboard ^ (bitboard - 1)) * debruijn64) >> 58]);
    }

    /**
     * @authors Kim Walisch, Mark Dickinson
     * @return index (0..63) of most significant one bit
     */
    // For search of the most significant bit
    constexpr Chess::Types::Square Reverse(Bitboard bitboard) {
        const Bitboard debruijn64 = 0x03f79d71b4cb0a89ULL;
        bitboard |= bitboard >> 1;
        bitboard |= bitboard >> 2;
        bitboard |= bitboard >> 4;
        bitboard |= bitboard >> 8;
        bitboard |= bitboard >> 16;
        bitboard |= bitboard >> 32;
        return static_cast<Chess::Types::Square>(bitScanIndex[(bitboard * debruijn64) >> 58]);
    }
}