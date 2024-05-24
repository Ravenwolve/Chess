#pragma once
#include <Bitboard.hpp>
#include <Move.hpp>
#include <vector>

namespace Chess::Core {
    class Position {
        BoardRepresentation _board;
        Types::Color _playerNow;
        Types::Square _enPassant;
        std::array<bool, 2> _shortCastlingPossible;
        std::array<bool, 2> _longCastlingPossible;
        std::array<bool, 2> _castlingHappened;
        uint16_t _moveCounter;
        uint8_t _rule50Counter;

        std::vector<Types::Move> _moves;

        void GenerateMovesForPiece(Types::Piece piece, Types::Square square);
    public:
        Position() noexcept;
        Position(const Position& other) noexcept;
        Position(const std::string& FEN);
        void Apply(Types::Move move);
        void GenerateMoves();
    };
}