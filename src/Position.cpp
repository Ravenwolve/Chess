#include <Position.hpp>
#include <sstream>
#include <BitScan.hpp>

using enum Chess::Types::Square;
using Chess::Types::File;
using Chess::Types::Rank;
using Chess::Types::Move;
using Chess::Types::Square;
using Chess::Types::Piece;
using Chess::Types::Color;
using Color::Black, Color::White;
using Piece::Pawn, Piece::Rook, Piece::Knight, Piece::Bishop, Piece::Queen, Piece::King;

Chess::Core::Position::Position() noexcept : _playerNow(White), _enPassant(Square::None), _rule50Counter(0), _moveCounter(1),
                                       _shortCastlingPossible{true, true}, _longCastlingPossible{true, true}, _castlingHappened {false, false} {
    _moves.reserve(218);
}

Chess::Core::Position::Position(const Position& other) noexcept : _playerNow(other._playerNow), _enPassant(other._enPassant), _rule50Counter(other._rule50Counter),
                                                         _moveCounter(other._moveCounter), _shortCastlingPossible(other._shortCastlingPossible),
                                                         _longCastlingPossible(other._longCastlingPossible), _castlingHappened(other._castlingHappened) {}

Chess::Core::Position::Position(const std::string& FEN) {
    _moves.reserve(218);
    std::string splittedFEN[4];
    std::stringstream ss(FEN);

    for (uint8_t i = 0UL; i < 4UL; ++i)
        ss >> splittedFEN[i];
    ss >> _rule50Counter >> _moveCounter;

    _shortCastlingPossible[White] = _shortCastlingPossible[Black] = _longCastlingPossible[White] = _longCastlingPossible[Black] = false;
    for (char ch : splittedFEN[2]) {
        switch (ch) {
            case 'K': _shortCastlingPossible[White] = true; break;
            case 'Q': _longCastlingPossible[White] = true; break;
            case 'k': _shortCastlingPossible[Black] = true; break;
            case 'q': _longCastlingPossible[Black] = true; break;
        }
    }
    
    _board = splittedFEN[0];
    _playerNow = splittedFEN[1][0] == 'w' ? White : Black;
    _enPassant = static_cast<Square>(splittedFEN[3][0] != '-' ? (Square)(splittedFEN[3][0] - 'a') + ((Square)(splittedFEN[3][1] - '0') << 3) : Square::None);
}

void Chess::Core::Position::Apply(Move move) {
    _board.AddPiece(move.attackerColor, move.attackerType, move.to);
    const uint8_t _1RANK = 8U, _2RANK = 16U, _1FILE = 1U;
    const Square oldEnPassant = _enPassant;
    if (move.targetColor != Color::None)
        _board.RemovePiece(move.targetColor, move.targetType, move.to);
    else ++_rule50Counter;
    if (move.attackerType == Pawn) {
        if (move.to == _enPassant)
            _board.RemovePiece(move.targetColor, move.attackerType, static_cast<Square>(move.attackerColor == White ? move.to - _1RANK : move.to + _1RANK));
        else if (move.to - move.from == _2RANK || move.from - move.to == _2RANK) {
            Square potentialEnPassant = static_cast<Square>(move.to + _1FILE);
            Bitboard pawns = _board.GetPieces((Color)!move.attackerColor, Pawn);
            if (pawns & 1ULL << potentialEnPassant)
                _enPassant = potentialEnPassant;
            else {
                potentialEnPassant = static_cast<Square>(move.to - _1FILE);
                if (pawns & 1ULL << potentialEnPassant)
                    _enPassant = potentialEnPassant;
            }
        }
        _rule50Counter = 0;
    }
    // проверка на троекратное повторение позиции (написать Zobrist-хэширование)
    // ...
    else if (move.attackerType == King) {
        _shortCastlingPossible[move.attackerColor] = _longCastlingPossible[move.attackerColor] = false;
        if (move.to - move.from == 2) {
            _board.AddPiece(move.attackerColor, Rook, static_cast<Square>(move.from + 1U));
            _board.RemovePiece(move.attackerColor, Rook, static_cast<Square>(move.from + 3U));
            _castlingHappened[move.attackerColor] = true;
        }
        else if (move.from - move.to == 2) {
            _board.AddPiece(move.attackerColor, Rook, static_cast<Square>(move.from - 1U));
            _board.RemovePiece(move.attackerColor, Rook, static_cast<Square>(move.from - 4U));
            _castlingHappened[move.attackerColor] = true;
        }
    }
    else if (move.attackerType == Rook) {
        if (_shortCastlingPossible[move.attackerColor] && move.from == (File::H | Rank::_1) || move.from == (File::H | Rank::_8))
            _shortCastlingPossible[move.attackerColor] = false;
        if (_longCastlingPossible[move.attackerColor] && move.from == (File::A | Rank::_1) || move.from == (File::A | Rank::_8))
            _longCastlingPossible[move.attackerColor] = false;
    }
    if (oldEnPassant == _enPassant)
        _enPassant = Square::None;
    ++_moveCounter;
    _playerNow = (Color)!_playerNow;
}

void Chess::Core::Position::GenerateMovesForPiece(Piece piece, Square square) {
    Bitboard movesMask, extraMask = 0ULL;
    Bitboard fileMask = [square % 8];
    Square defender;
    switch (piece) {
        case Rook: {
            
        }
        case Knight: {
            movesMask = _board.GetKnightAttacks(_playerNow, square);
            break;
        }
        case Bishop: {

        }
        case Queen: {

        }
        case King: {
            movesMask = _board.GetKingAttacks(_playerNow, square);
            break;
        }
        default: {
            movesMask = _board.GetPawnAttacks(_playerNow, square);
            extraMask = _board.GetPawnAdvances(_playerNow, square);
        }
    }
    

}

void Chess::Core::Position::GenerateMoves() {
    Bitboard allPawns = _board.GetPieces(_playerNow, Pawn);
    Bitboard allRooks = _board.GetPieces(_playerNow, Rook);
    Bitboard allKnights = _board.GetPieces(_playerNow, Knight);
    Bitboard allBishops = _board.GetPieces(_playerNow, Bishop);
    Bitboard allQueens = _board.GetPieces(_playerNow, Queen);
    Bitboard king = _board.GetPieces(_playerNow, King);
    Square attacker;

    while (allPawns) {
        attacker = BitScan::Forward(allPawns);
        Set<0>(allPawns, attacker);
        
    }
}