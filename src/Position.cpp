#include <Position.hpp>
#include <sstream>
#include <BitScan.hpp>
#include <Masks.hpp>
#include <functional>

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
    if (move.targetColor != Color::NoneColor)
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
        if (_shortCastlingPossible[move.attackerColor] && move.from == (Rank::_1 << File::H) || move.from == (Rank::_8 << File::H))
            _shortCastlingPossible[move.attackerColor] = false;
        if (_longCastlingPossible[move.attackerColor] && move.from == (Rank::_1 << File::A) || move.from == (Rank::_8 << File::A))
            _longCastlingPossible[move.attackerColor] = false;
    }
    if (oldEnPassant == _enPassant)
        _enPassant = Square::None;
    ++_moveCounter;
    _playerNow = (Color)!_playerNow;
}

// Unreadable because of optimization
void Chess::Core::Position::GenerateMovesForPiece(Piece piece, Square square) {
    uint8_t x = square % 8, y = square / 8;
    Bitboard movesMask = 0ULL;

    // Searching attacked squares and defenders on the board
    std::function SearchPieces = [&square, &piece](std::vector<Move>& moves, Bitboard movesMask, bool searchDefenders, Color player, BoardRepresentation& board)->void {
        Square target;
        Piece targetPiece = Piece::NonePiece;
        Color targetColor = Color::NoneColor;
        Bitboard targetMask;
        while (movesMask) {
            target = BitScan::Forward(movesMask);
            Set<0>(movesMask, target);
            targetMask = 1ULL << target;
            if (searchDefenders) {
                targetPiece = Piece::NonePiece;
                targetColor = Color::NoneColor; 
                for (uint8_t i = 0U; i < 6; ++i)
                    if (targetMask & board.GetPieces(static_cast<Color>(!player), static_cast<Piece>(i))) {
                        targetPiece = static_cast<Piece>(i);
                        targetColor = static_cast<Color>(!player);
                    }
            }
            moves.push_back({square, target, piece, targetPiece, player, targetColor});
        }
    };
    // Searching defenders on the board for sliding pieces (sliding piece have between 0 and 4 attacked pieces)
    std::function SearchExtremePieces = [&square, &piece](std::vector<Move>& moves, Bitboard& movesMask, Color player, BoardRepresentation& board, bool isRook)->void {
        uint8_t x = square % 8, y = square / 8;
        Bitboard vertical = isRook ? movesMask & Masks::files[x] : movesMask & Masks::diagonalRays[0][7 - x + y];
        Bitboard horizontal = isRook ? movesMask & Masks::ranks[y] : movesMask & Masks::diagonalRays[1][x + y];
        Square upExtreme = None, lowExtreme = None, leftExtreme = None, rightExtreme = None;
        Piece upPiece = Piece::NonePiece, lowPiece = Piece::NonePiece, leftPiece = Piece::NonePiece, rightPiece = Piece::NonePiece;
        if (vertical) {
            lowExtreme = BitScan::Forward(vertical);
            Set<0>(vertical, lowExtreme);
        }
        if (vertical) {
            upExtreme = BitScan::Reverse(vertical);
            Set<0>(vertical, upExtreme);
        }
        if (horizontal) {
            leftExtreme = BitScan::Forward(horizontal);
            Set<0>(horizontal, leftExtreme);
        }
        if (horizontal) {
            rightExtreme = BitScan::Reverse(horizontal);
            Set<0>(horizontal, rightExtreme);
        }
        Bitboard upMask = 1ULL << upExtreme, lowMask = 1ULL << lowExtreme, leftMask = 1ULL << leftExtreme, rightMask = 1ULL << rightExtreme;
        Bitboard targetPieceMask;
        for (uint8_t i = 0U; i < 6; ++i) {
            targetPieceMask = board.GetPieces(static_cast<Color>(!player), static_cast<Piece>(i));
            if (upMask & targetPieceMask)
                upPiece = static_cast<Piece>(i);
            else if (lowMask & targetPieceMask)
                lowPiece = static_cast<Piece>(i);
            else if (leftMask & targetPieceMask)
                leftPiece = static_cast<Piece>(i);
            else if (lowMask & targetPieceMask)
                rightPiece = static_cast<Piece>(i);
        }
        moves.push_back({square, upExtreme, piece, upPiece, player, upPiece != Piece::NonePiece ? static_cast<Color>(!player) : Color::NoneColor});
        moves.push_back({square, lowExtreme, piece, lowPiece, player, lowPiece != Piece::NonePiece ? static_cast<Color>(!player) : Color::NoneColor});
        moves.push_back({square, leftExtreme, piece, leftPiece, player, leftPiece != Piece::NonePiece ? static_cast<Color>(!player) : Color::NoneColor});
        moves.push_back({square, rightExtreme, piece, rightPiece, player, rightPiece != Piece::NonePiece ? static_cast<Color>(!player) : Color::NoneColor});
        movesMask = vertical | horizontal;
    };

    switch (piece) {
        case Rook: {
            movesMask = _board.GetRookAttacks(_playerNow, square);
            SearchExtremePieces(_moves, movesMask, _playerNow, _board, true);
            SearchPieces(_moves, movesMask, false, _playerNow, _board);
            break;
        }
        case Bishop: {
            movesMask = _board.GetBishopAttacks(_playerNow, square);
            SearchExtremePieces(_moves, movesMask, _playerNow, _board, false);
            SearchPieces(_moves, movesMask, false, _playerNow, _board);
            break;
        }
        case Queen: {
            movesMask = _board.GetRookAttacks(_playerNow, square);
            SearchExtremePieces(_moves, movesMask, _playerNow, _board, true);
            SearchPieces(_moves, movesMask, false, _playerNow, _board);
            movesMask = _board.GetBishopAttacks(_playerNow, square);
            SearchExtremePieces(_moves, movesMask, _playerNow, _board, false);
            SearchPieces(_moves, movesMask, false, _playerNow, _board);
            break;
        }
        case Knight: {
            SearchPieces(_moves, _board.GetKnightAttacks(_playerNow, square), true, _playerNow, _board);
            break;
        }
        case King: {
            SearchPieces(_moves, _board.GetKingAttacks(_playerNow, square), true, _playerNow, _board);
            break;
        }
        default: {
            SearchPieces(_moves, _board.GetPawnAttacks(_playerNow, square), true, _playerNow, _board);
            SearchPieces(_moves, _board.GetPawnAdvances(_playerNow, square), false, _playerNow, _board);
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
        GenerateMovesForPiece(Pawn, attacker);
    }
    while (allRooks) {
        attacker = BitScan::Forward(allRooks);
        Set<0>(allRooks, attacker);
        GenerateMovesForPiece(Rook, attacker);
    }
    while (allKnights) {
        attacker = BitScan::Forward(allKnights);
        Set<0>(allKnights, attacker);
        GenerateMovesForPiece(Knight, attacker);
    }
    while (allBishops) {
        attacker = BitScan::Forward(allBishops);
        Set<0>(allBishops, attacker);
        GenerateMovesForPiece(Bishop, attacker);
    }
    while (allQueens) {
        attacker = BitScan::Forward(allQueens);
        Set<0>(allQueens, attacker);
        GenerateMovesForPiece(Queen, attacker);
    }
    GenerateMovesForPiece(Knight, BitScan::Forward(king));
}