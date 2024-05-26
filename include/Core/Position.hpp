#pragma once
#include <Core/Bitboard.hpp>
#include <Core/Move.hpp>
#include <vector>

namespace Chess::Core {
    class Position {
    protected:
        BoardRepresentation _board;
        Types::Color _playerNow;
        Types::Square _enPassant;
        std::array<bool, 2> _shortCastlingPossible;
        std::array<bool, 2> _longCastlingPossible;
        std::array<bool, 2> _castlingHappened;
        bool _isCheck;
        uint16_t _moveCounter;
        uint8_t _rule50Counter;

        std::vector<Types::Move> _moves;
        void GenerateEnPassantMove(Types::Square square);
        void GenerateShortCastlingMove(Types::Color color);
        void GenerateLongCastlingMove(Types::Color color);
    public:
        Position() noexcept;
        Position(const Position& other) noexcept;
        Position(const std::string& FEN);
        void Apply(const Types::Move& move);
        void GenerateMovesForPiece(Types::Piece piece, Types::Square square);
        void GenerateMoves();
        bool IsLegal(const Types::Move& move);
        bool IsCheck();
        bool NoMoves();
        bool InMoves(Types::Square target);
        std::pair<Types::Color, Types::Piece> GetPiece(Types::Square square);
        Types::Color PlayerNow() noexcept;
        Types::Square GetEnPassant() noexcept;
    };
}