#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <bitset>
#include <sstream>
#include <tuple>

namespace Chess {

    enum Color {
        White,
        Black
    };

    enum Piece {
        Pawn,
        Rook,
        Knight,
        Bishop,
        Queen,
        King
    };

    enum Rank {
        _1 = (uint8_t)0,
        _2 = (uint8_t)8,
        _3 = (uint8_t)16,
        _4 = (uint8_t)24,
        _5 = (uint8_t)32,
        _6 = (uint8_t)40,
        _7 = (uint8_t)48,
        _8 = (uint8_t)56
    };

    enum File {
        A = (uint8_t)0,
        B = (uint8_t)1,
        C = (uint8_t)2,
        D = (uint8_t)3,
        E = (uint8_t)4,
        F = (uint8_t)5,
        G = (uint8_t)6,
        H = (uint8_t)7
    };

    namespace Core {
        using Bitboard = uint64_t;
        using Castling = std::bitset<4>;
        using EnPassant = Bitboard;
        using MovesWithoutCapturing = uint8_t;
        using NumberOfMove = uint8_t;
        using ShortFEN = std::string;

        bool Get(const Bitboard &obj, const uint8_t bitNumber);

        template <bool bitValue>
        void Set(Bitboard &obj, const uint8_t bitNumber);

        std::tuple<ShortFEN, Color, Castling, EnPassant, MovesWithoutCapturing, NumberOfMove> ParseFEN(const std::string &FEN);

        class BoardRepresentation {
        private:
            std::array<std::array<Bitboard, 6>, 2> _pieces;

            std::array<Bitboard, 2> _sides;
            Bitboard _unionAll;

            void UpdateBitboards() noexcept;

        public:
            BoardRepresentation() noexcept;
            BoardRepresentation(const BoardRepresentation &other) noexcept;
            BoardRepresentation(const std::string &shortFEN);

            Bitboard GetPawnAttacks(Color color, Bitboard bitboard) noexcept;
            Bitboard GetPawnAdvances(Color color, Bitboard bitboard) noexcept;
            Bitboard GetKnightAttacks(Bitboard bitboard) noexcept;
            Bitboard GetKingAttacks(Bitboard bitboard) noexcept;
            Bitboard GetRookAttacks(uint8_t square);
            Bitboard GetBishopAttacks(uint8_t square);
            Bitboard GetQueenAttacks(uint8_t square);
        };
    }
}