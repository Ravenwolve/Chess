#pragma once
#include <array>
#include <Types.hpp>

namespace Chess::Core::Masks {
       static constexpr std::array<Bitboard, 8> ranks {
        0b11111111ULL,
        0b11111111ULL << 8,
        0b11111111ULL << 16,
        0b11111111ULL << 24,
        0b11111111ULL << 32,
        0b11111111ULL << 40,
        0b11111111ULL << 48,
        0b11111111ULL << 56
    };
    static constexpr std::array<Bitboard, 8> files {
        1ULL   | (1ULL << 8)   | (1ULL << 16)   | (1ULL << 24)   | (1ULL << 32)   | (1ULL << 40)   | (1ULL << 48)   | (1ULL << 56),
        2ULL   | (2ULL << 8)   | (2ULL << 16)   | (2ULL << 24)   | (2ULL << 32)   | (2ULL << 40)   | (2ULL << 48)   | (2ULL << 56),
        4ULL   | (4ULL << 8)   | (4ULL << 16)   | (4ULL << 24)   | (4ULL << 32)   | (4ULL << 40)   | (4ULL << 48)   | (4ULL << 56),
        8ULL   | (8ULL << 8)   | (8ULL << 16)   | (8ULL << 24)   | (8ULL << 32)   | (8ULL << 40)   | (8ULL << 48)   | (8ULL << 56),
        16ULL  | (16ULL << 8)  | (16ULL << 16)  | (16ULL << 24)  | (16ULL << 32)  | (16ULL << 40)  | (16ULL << 48)  | (16ULL << 56),
        32ULL  | (32ULL << 8)  | (32ULL << 16)  | (32ULL << 24)  | (32ULL << 32)  | (32ULL << 40)  | (32ULL << 48)  | (32ULL << 56),
        64ULL  | (64ULL << 8)  | (64ULL << 16)  | (64ULL << 24)  | (64ULL << 32)  | (64ULL << 40)  | (64ULL << 48)  | (64ULL << 56),
        128ULL | (128ULL << 8) | (128ULL << 16) | (128ULL << 24) | (128ULL << 32) | (128ULL << 40) | (128ULL << 48) | (128ULL << 56)
    };

    static constexpr std::array<std::array<Bitboard, 15>, 2> diagonalRays = {
        // -> //
        std::array<Bitboard, 15> {
            1ULL << 7,
            1ULL << 6 | 1ULL << (7 + 8),
            1ULL << 5 | 1ULL << (6 + 8) | 1ULL << (7 + 16),
            1ULL << 4 | 1ULL << (5 + 8) | 1ULL << (6 + 16) | 1ULL << (7 + 24),
            1ULL << 3 | 1ULL << (4 + 8) | 1ULL << (5 + 16) | 1ULL << (6 + 24) | 1ULL << (7 + 32),
            1ULL << 2 | 1ULL << (3 + 8) | 1ULL << (4 + 16) | 1ULL << (5 + 24) | 1ULL << (6 + 32) | 1ULL << (7 + 40),
            1ULL << 1 | 1ULL << (2 + 8) | 1ULL << (3 + 16) | 1ULL << (4 + 24) | 1ULL << (5 + 32) | 1ULL << (6 + 40) | 1ULL << (7 + 48),  
            1ULL      | 1ULL << (1 + 8) | 1ULL << (2 + 16) | 1ULL << (3 + 24) | 1ULL << (4 + 32) | 1ULL << (5 + 40) | 1ULL << (6 + 48) | 1ULL << (7 + 56),
                        1ULL << 8       | 1ULL << (1 + 16) | 1ULL << (2 + 24) | 1ULL << (3 + 32) | 1ULL << (4 + 40) | 1ULL << (5 + 48) | 1ULL << (6 + 56),
                                          1ULL << 16       | 1ULL << (1 + 24) | 1ULL << (2 + 32) | 1ULL << (3 + 40) | 1ULL << (4 + 48) | 1ULL << (5 + 56),
                                                             1ULL << 24       | 1ULL << (1 + 32) | 1ULL << (2 + 40) | 1ULL << (3 + 48) | 1ULL << (4 + 56),
                                                                                1ULL << 32       | 1ULL << (1 + 40) | 1ULL << (2 + 48) | 1ULL << (3 + 56),
                                                                                                   1ULL << 40       | 1ULL << (1 + 48) | 1ULL << (2 + 56),
                                                                                                                      1ULL << 48       | 1ULL << (1 + 56),
                                                                                                                                         1ULL << 56
        },
        // -> \\.
        std::array<Bitboard, 15> {
            1ULL,
            1ULL << 1 | 1ULL << 8,
            1ULL << 2 | 1ULL << (1 + 8) | 1ULL << 16,
            1ULL << 3 | 1ULL << (2 + 8) | 1ULL << (1 + 16) | 1ULL << 24,
            1ULL << 4 | 1ULL << (3 + 8) | 1ULL << (2 + 16) | 1ULL << (1 + 24) | 1ULL << 32,
            1ULL << 5 | 1ULL << (4 + 8) | 1ULL << (3 + 16) | 1ULL << (2 + 24) | 1ULL << (1 + 32) | 1ULL << 40,
            1ULL << 6 | 1ULL << (5 + 8) | 1ULL << (4 + 16) | 1ULL << (3 + 24) | 1ULL << (2 + 32) | 1ULL << (1 + 40) | 1ULL << 48,  
            1ULL << 7 | 1ULL << (6 + 8) | 1ULL << (5 + 16) | 1ULL << (4 + 24) | 1ULL << (3 + 32) | 1ULL << (2 + 40) | 1ULL << (1 + 48) | 1ULL << 56,
                        1ULL << (7 + 8) | 1ULL << (6 + 16) | 1ULL << (5 + 24) | 1ULL << (4 + 32) | 1ULL << (3 + 40) | 1ULL << (2 + 48) | 1ULL << (1 + 56),
                                          1ULL << (7 + 16) | 1ULL << (6 + 24) | 1ULL << (5 + 32) | 1ULL << (4 + 40) | 1ULL << (3 + 48) | 1ULL << (2 + 56),
                                                             1ULL << (7 + 24) | 1ULL << (6 + 32) | 1ULL << (5 + 40) | 1ULL << (4 + 48) | 1ULL << (3 + 56),
                                                                                1ULL << (7 + 24) | 1ULL << (6 + 40) | 1ULL << (5 + 48) | 1ULL << (4 + 56),
                                                                                                   1ULL << (7 + 40) | 1ULL << (6 + 48) | 1ULL << (5 + 56),
                                                                                                                      1ULL << (7 + 48) | 1ULL << (6 + 56),
                                                                                                                                         1ULL << (7 + 56)
        }
    };
}