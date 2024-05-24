#pragma once
#include <array>
#include <Types.hpp>
#include <Masks.hpp>

namespace Chess::Core::Cache::Calc {
    static consteval std::array<Bitboard, 64> CalculateRaysForRook() {
        using Types::File::A, Types::File::H;
        std::array<Bitboard, 64> masks;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;

        for (uint8_t i = 0U; i < masks.size(); ++i)
            masks[i] = (~(1ULL << i) & (Masks::ranks[i / 8] | Masks::files[i % 8])) &
            ~((i > 8 ? Masks::ranks[0] : 0ULL) | (i < 56 ? Masks::ranks[7] : 0ULL) |
            (i % 8 ? Masks::files[A] : 0ULL) | ((i + 1) % 8 ? Masks::files[H] : 0ULL));

        return masks;
    }

    static consteval std::array<Bitboard, 64> CalculateRaysForBishop() {
        using Types::File::A, Types::File::H;
        std::array<Bitboard, 64> masks;
        Bitboard sourceBit;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;
        const Bitboard notFrame = ~(Masks::ranks[0] | Masks::ranks[7] | Masks::files[A] | Masks::files[H]);

        for (uint8_t i = 0U; i < masks.size(); ++i) {
            sourceBit = 1ULL << i;
            masks[i] = 0;
            for (uint8_t j = 0U; j < 15U; ++j)
                if (sourceBit & Masks::diagonalRays[0][j]) {
                    masks[i] |= ~sourceBit & Masks::diagonalRays[0][j];
                    break;
                }
            for (uint8_t j = 0U; j < 15U; ++j)
                if (sourceBit & Masks::diagonalRays[1][j]) {
                    masks[i] |= ~sourceBit & Masks::diagonalRays[1][j];
                    break;
                }
            masks[i] &= notFrame;
        }
        return masks;
    }

    static consteval std::array<Bitboard, 64> CalculateForKnight() {
        using Types::File::A, Types::File::B, Types::File::G, Types::File::H;
        std::array<Bitboard, 64> masks;
        Bitboard tmp, lr1, lr2;
        for (uint8_t sq = 0; sq < 64; ++sq) {
            tmp = 1ULL << sq;
            lr1 = tmp >> 1 & ~Masks::files[H] | tmp << 1 & ~Masks::files[A];
            lr2 = tmp >> 2 & ~(Masks::files[G] | Masks::files[H]) |
                       tmp << 2 & ~(Masks::files[A] | Masks::files[B]);
            masks[sq] = lr1 << 16 | lr1 >> 16 | lr2 << 8 | lr2 >> 8;
        }
        return masks;
    }

    static consteval std::array<Bitboard, 64> CalculateForKing() {
        using Types::File::A, Types::File::H;
        std::array<Bitboard, 64> masks;
        Bitboard tmp, lr;
        for (uint8_t sq = 0; sq < 64; ++sq) {
            tmp = 1ULL << sq;
            lr = tmp >> 1 & ~Masks::files[H] | tmp << 1 & ~Masks::files[A];
            masks[sq] = lr | lr << 8 | lr >> 8 | tmp << 8 | tmp >> 8;
        }
        return masks;
    }
}