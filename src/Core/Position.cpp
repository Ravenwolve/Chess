#include <Core/Position.hpp>
#include <sstream>
#include <Core/BitScan.hpp>
#include <Core/Masks.hpp>
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
                                       _shortCastlingPossible{true, true}, _longCastlingPossible{true, true}, _castlingHappened {false, false}, _isCheck(false)  {
    _moves.reserve(218);
}

Chess::Core::Position::Position(const Position& other) noexcept : _playerNow(other._playerNow), _enPassant(other._enPassant), _rule50Counter(other._rule50Counter),
                                                         _moveCounter(other._moveCounter), _shortCastlingPossible(other._shortCastlingPossible),
                                                         _longCastlingPossible(other._longCastlingPossible), _castlingHappened(other._castlingHappened),
                                                         _board(other._board), _isCheck(other._isCheck) {}

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
    _isCheck = IsCheck();
}

void Chess::Core::Position::Apply(const Move& move) {
    using enum Types::File;
    _board.RemovePiece(move.attackerColor, move.attackerType, move.from);
    if (move.targetColor != Color::NoneColor)
        _board.RemovePiece(move.targetColor, move.targetType, move.to);
    else ++_rule50Counter;
    _board.AddPiece(move.attackerColor, move.attackerType, move.to);
    const uint8_t _1RANK = 8U, _2RANK = 16U, _1FILE = 1U;
    const Square oldEnPassant = _enPassant;
    if (move.attackerType == Pawn) {
        if (move.to == _enPassant)
            _board.RemovePiece(move.targetColor, move.targetType, static_cast<Square>(move.attackerColor == White ? move.to - _1RANK : move.to + _1RANK));
        else if (move.to - move.from == _2RANK || move.from - move.to == _2RANK) {
            Bitboard pawns = _board.GetPieces(static_cast<Color>(!move.attackerColor), Pawn);
            if (((1ULL << move.to) << _1FILE | (1ULL << move.to) >> _1FILE) & pawns & ~(Masks::files[A] | Masks::files[H]))
                _enPassant = move.attackerColor == White ? static_cast<Square>(move.to - _1RANK) : static_cast<Square>(move.to + _1RANK);
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
    _playerNow = static_cast<Color>(!_playerNow);
    // for check the king
    if (move.from != move.to)
        _isCheck = IsCheck();
    _moves.clear();
}

bool Chess::Core::Position::IsLegal(const Move& move) {
    Position copy = *this;
    copy.Apply(move);
    Bitboard kingMask = copy._board.GetPieces(move.attackerColor, King); // 10000

    std::function GetAttacks = [&copy](Color color, Piece piece, Square square)->Bitboard {
        switch (piece) {
            case Queen: return copy._board.GetQueenAttacks(color, square);
            case Rook: return copy._board.GetRookAttacks(color, square);
            case Bishop: return copy._board.GetBishopAttacks(color, square);
            case Knight: return copy._board.GetKnightAttacks(color, square);
            case Pawn: return copy._board.GetPawnAttacks(color, square);
            case King: return copy._board.GetKingAttacks(color, square);
            default: return 0ULL;
        }
    };

    std::function BruteForcePieces = [&copy, &GetAttacks](Bitboard attackMask, Bitboard kingMask, Color color, Piece piece, bool& isIllegalExactly)->void {
        Square square;
        while (attackMask) {
            square = BitScan::Forward(attackMask);
            Set<0>(attackMask, square);
            if (GetAttacks(color, piece, square) & kingMask) {
                isIllegalExactly = true;
                break;
            }
        }
    };

    bool isIllegalExactly = false;
    for (uint8_t piece = 0; piece < 6; ++piece) {
        BruteForcePieces(copy._board.GetPieces(static_cast<Color>(!move.attackerColor), static_cast<Piece>(piece)),
                        kingMask, static_cast<Color>(!move.attackerColor), static_cast<Piece>(piece), isIllegalExactly);
        if (isIllegalExactly)
            return false;
    }
    return true;
}

// Unreadable because of optimization
void Chess::Core::Position::GenerateMovesForPiece(Piece piece, Square square) {
    uint8_t x = square % 8, y = square / 8;
    Bitboard movesMask = 0ULL;

    // Searching attacked squares and defenders on the board
    std::function SearchPieces = [&square, &piece, this](std::vector<Move>& moves, Bitboard movesMask, bool searchDefenders, Color player, BoardRepresentation& board)->void {
        Square target;
        Piece targetPiece = Piece::NonePiece;
        Color targetColor = Color::NoneColor;
        Bitboard targetMask;
        Move move;
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
            move = {square, target, piece, targetPiece, player, targetColor};
            if (IsLegal(move))
                moves.push_back(move);
        }
    };
    // Searching defenders on the board for sliding pieces (sliding piece have between 0 and 4 attacked pieces)
    std::function SearchExtremePieces = [&square, &piece, this](std::vector<Move>& moves, Bitboard& movesMask, Color player, BoardRepresentation& board, bool isRook)->void {
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
        Move move = {square, upExtreme, piece, upPiece, player, upPiece != Piece::NonePiece ? static_cast<Color>(!player) : Color::NoneColor};
        if (IsLegal(move))
            moves.push_back(move);
        move = {square, lowExtreme, piece, lowPiece, player, lowPiece != Piece::NonePiece ? static_cast<Color>(!player) : Color::NoneColor};
        if (IsLegal(move))
            moves.push_back(move);
        move = {square, leftExtreme, piece, leftPiece, player, leftPiece != Piece::NonePiece ? static_cast<Color>(!player) : Color::NoneColor};
        if (IsLegal(move))
            moves.push_back(move);
        move = {square, rightExtreme, piece, rightPiece, player, rightPiece != Piece::NonePiece ? static_cast<Color>(!player) : Color::NoneColor};
        if (IsLegal(move))
            moves.push_back(move);
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
            GenerateShortCastlingMove(_playerNow);
            GenerateLongCastlingMove(_playerNow);
            break;
        }
        default: {
            SearchPieces(_moves, _board.GetPawnAttacks(_playerNow, square), true, _playerNow, _board);
            SearchPieces(_moves, _board.GetPawnAdvances(_playerNow, square), false, _playerNow, _board);
            GenerateEnPassantMove(square);
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

std::pair<Color, Piece> Chess::Core::Position::GetPiece(Types::Square square) {
    Bitboard mask = 1ULL << square;
    for (uint8_t c = 0U; c < 2U; ++c)
        for (uint8_t p = 0U; p < 6U; ++p)
            if (_board.GetPieces(static_cast<Color>(c), static_cast<Piece>(p)) & mask)
                return { static_cast<Color>(c), static_cast<Piece>(p) };
    return { Color::NoneColor, Piece::NonePiece };
}

Chess::Types::Color Chess::Core::Position::PlayerNow() noexcept { 
    return _playerNow;
}

bool Chess::Core::Position::InMoves(Types::Square target) {
    for (Move& move : _moves)
        if (move.to == target)
            return true;
    return false;
}

void Chess::Core::Position::GenerateEnPassantMove(Types::Square square) {
    using enum File;
    if (_enPassant != Square::None) {
        Move move;
        if (PlayerNow() == White && (1ULL << _enPassant & ((1ULL << square) << 7 & ~Masks::files[A] | (1ULL << square) << 9 & ~Masks::files[H])))
            move = {square, _enPassant, Pawn, Pawn, White, Black};
        else if (1ULL << _enPassant & ((1ULL << square) >> 7 & ~Masks::files[A] | (1ULL << square) >> 9 & ~Masks::files[H]))
            move = {square, _enPassant, Pawn, Pawn, Black, White};
        if (IsLegal(move))
            _moves.push_back(move);
    }
}

void Chess::Core::Position::GenerateShortCastlingMove(Types::Color color) {
    if (!_shortCastlingPossible[color] || _isCheck)
        return;
    Square r1, r2, king;
    if (color == White)
        king = E1, r1 = F1, r2 = G1;
    else king = E8, r1 = F8, r2 = G8;
    std::pair near = GetPiece(r1), far = GetPiece(r2);
    if (near.first == Color::NoneColor && far.first == Color::NoneColor &&
        IsLegal({king, r1, King, Piece::NonePiece, color, Color::NoneColor})) {
        Move shortCastling = {king, r2, King, Piece::NonePiece, color, Color::NoneColor};
        if (IsLegal(shortCastling))
            _moves.push_back(shortCastling);
    }
}

void Chess::Core::Position::GenerateLongCastlingMove(Types::Color color) {
    if (!_longCastlingPossible[color] || _isCheck)
        return;
    Square l1, l2, l3, king;
    if (color == White)
        king = E1, l1 = D1, l2 = C1, l3 = B1;
    else king = E8, l1 = D8, l2 = C8, l3 = B8;
    std::pair near = GetPiece(l1), far = GetPiece(l2), veryFar = GetPiece(l3);
    if (near.first == Color::NoneColor && far.first == Color::NoneColor && veryFar.first == Color::NoneColor &&
    IsLegal({king, l1, King, Piece::NonePiece, color, Color::NoneColor})) {
        Move shortCastling = {king, l2, King, Piece::NonePiece, color, Color::NoneColor};
        if (IsLegal(shortCastling))
            _moves.push_back(shortCastling);
    }
}

Chess::Types::Square Chess::Core::Position::GetEnPassant() noexcept {
    return _enPassant;
}

bool Chess::Core::Position::IsCheck() {
    Square king = BitScan::Forward(_board.GetPieces(_playerNow, King));
    return !IsLegal({king, king, King, King, _playerNow, _playerNow});
}

bool Chess::Core::Position::NoMoves() {
    GenerateMoves();
    bool noMoves = _moves.size() == 0;
    _moves.clear();
    return noMoves;
}