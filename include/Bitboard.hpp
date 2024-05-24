#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <Types.hpp>

namespace Chess {
    namespace Core {
        bool Get(const Bitboard &obj, const uint8_t bitNumber);

        template <bool bitValue>
        void Set(Bitboard &obj, const uint8_t bitNumber);

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

            Bitboard GetPawnAttacks(Types::Color color, Types::Square square);
            Bitboard GetPawnAdvances(Types::Color color, Types::Square square);
            Bitboard GetKnightAttacks(Types::Color color, Types::Square square);
            Bitboard GetKingAttacks(Types::Color color, Types::Square square);
            Bitboard GetRookAttacks(Types::Color color, Types::Square square);
            Bitboard GetBishopAttacks(Types::Color color, Types::Square square);
            Bitboard GetQueenAttacks(Types::Color color, Types::Square square);

            void AddPiece(Types::Color color, Types::Piece piece, Types::Square square);
            void RemovePiece(Types::Color color, Types::Piece piece, Types::Square square);
            Bitboard GetPieces(Types::Color color, Types::Piece piece);
        };
    }
}