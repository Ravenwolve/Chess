#include <Core/Bitboard.hpp>
#include <Core/BitScan.hpp>
#include <Core/Cache.hpp>
#include <Core/Masks.hpp>

using Chess::Types::File;
using Chess::Types::Rank;
using Chess::Types::Square;
using Chess::Types::Piece;
using Chess::Types::Color;
using Color::Black, Color::White;
using Piece::Pawn, Piece::Rook, Piece::Knight, Piece::Bishop, Piece::Queen, Piece::King;

// There is issue with bitNumber > 63 because of checking disabled for better performance
bool Chess::Core::Get(const Bitboard& obj, const uint8_t bitNumber) {
    return obj & (1ULL << bitNumber);
}

template <>
void Chess::Core::Set<1>(Bitboard& obj, const uint8_t bitNumber) {
    obj |= 1ULL << bitNumber;
}

template<>
void Chess::Core::Set<0>(Bitboard& obj, const uint8_t bitNumber) {
    obj &= ~(1ULL << bitNumber);
}

Chess::Core::BoardRepresentation::BoardRepresentation()
    noexcept : _pieces {
        std::array<Bitboard, 6>{
            0b11111111ULL << 8, // Pawns / FEN: P
            0b10000001ULL,      // Rooks / FEN: R
            0b01000010ULL,      // Knights / FEN: N
            0b00100100ULL,      // Bishops / FEN: B
            0b00001000ULL,      // Queen / FEN: Q
            0b00010000ULL      // King / FEN: K
        },
        std::array<Bitboard, 6>{
            0b11111111ULL << 48, // Pawns / FEN: p
            0b10000001ULL << 56, // Rooks / FEN: r
            0b01000010ULL << 56, // Knights / FEN: n
            0b00100100ULL << 56, // Bishops / FEN: b
            0b00001000ULL << 56, // Queen / FEN: q
            0b00010000ULL << 56 // King / FEN: k
        }
    }, _sides {
        _pieces[White][Pawn] | _pieces[White][Rook] | _pieces[White][Knight] | _pieces[White][Bishop] | _pieces[White][Queen] | _pieces[White][King],
        _pieces[Black][Pawn] | _pieces[Black][Rook] | _pieces[Black][Knight] | _pieces[Black][Bishop] | _pieces[Black][Queen] | _pieces[Black][King]
    }, _unionAll(_sides[White] | _sides[Black]) { }

Chess::Core::BoardRepresentation::BoardRepresentation(const BoardRepresentation &other) noexcept = default;

Chess::Core::BoardRepresentation::BoardRepresentation(const std::string& shortFEN) {
    for (size_t i = 0UL; i < 12UL; ++i)
        _pieces[White][i] = _pieces[Black][i] = 0;
    uint8_t x = 0, y = 7;
    Color color;
    for (char ch : shortFEN) {
        if (std::isdigit(ch))
            x += ch - '0';
        else if (ch == '/') {
            x = 0;
            --y;
        }
        else {
            if (std::isupper(ch)) {
                color = White;
                ch = std::tolower(ch);
            }
            else color = Black;

            switch (ch) {                               // y << 3 equals y * 8
                case 'r': _pieces[color][Rook] |= 1ULL << (y << 3) + x; break;
                case 'n': _pieces[color][Knight] |= 1ULL << (y << 3) + x; break;
                case 'b': _pieces[color][Bishop] |= 1ULL << (y << 3) + x; break;
                case 'q': _pieces[color][Queen] |= 1ULL << (y << 3) + x; break;
                case 'k': _pieces[color][King] |= 1ULL << (y << 3) + x; break;
                case 'p': _pieces[color][Pawn] |= 1ULL << (y << 3) + x; break;
            }

            ++x;
        }
    }

    UpdateBitboards();
}

void Chess::Core::BoardRepresentation::UpdateBitboards() noexcept {
    _sides[Black] = _sides[White] = 0;
    for (Bitboard item : _pieces[White])
        _sides[White] |= item;
    for (Bitboard item : _pieces[Black])
        _sides[Black] |= item;

    _unionAll = _sides[White] | _sides[Black];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetPawnAttacks(Color color, Square square) {
    if (color == White)
        return ((1ULL << square) << 7) & ~Masks::files[File::H] | ((1ULL << square) << 9) & ~Masks::files[File::A] & _sides[Black];
    else return ((1ULL << square) >> 7) & ~Masks::files[File::H] | ((1ULL << square) >> 9) & ~Masks::files[File::A] & _sides[White];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetPawnAdvances(Color color, Square square) {
    Bitboard advance = (color == White ? (1ULL << square) << 8 : ((1ULL << square) >> 8)) & ~_unionAll; // Short
    advance |= (color == White ? (advance & Rank::_3) << 8 : ((advance & Rank::_6) >> 8)) & ~_unionAll; // Long
    return advance;
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetKnightAttacks(Color color, Square square) {
    return Cache::GetCache(Knight, square) & ~_sides[color];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetKingAttacks(Color color, Square square) {
    return Cache::GetCache(King, square) & ~_sides[color];
}

/**
    Magic bitboards method for sliding pieces
    @link (@https://www.chessprogramming.org/Magic_Bitboards) for description
*/
Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetRookAttacks(Color color, Square square) {
    return Cache::GetCache(Rook, square, Cache::Hash(square, _unionAll & Cache::GetSlidingMask(Rook, square), Rook)) & ~_sides[color];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetBishopAttacks(Color color, Square square) {
    return Cache::GetCache(Bishop, square, Cache::Hash(square, _unionAll & Cache::GetSlidingMask(Bishop, square), Bishop)) & ~_sides[color];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetQueenAttacks(Color color, Square square) {
    return GetBishopAttacks(color, square) | GetRookAttacks(color, square);
}

void Chess::Core::BoardRepresentation::AddPiece(Color color, Piece piece, Square square) {
    _pieces[color][piece] |= 1ULL << square;
    UpdateBitboards();
}

void Chess::Core::BoardRepresentation::RemovePiece(Color color, Piece piece, Square square) {
    _pieces[color][piece] &= ~(1ULL << square);
    UpdateBitboards();
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetPieces(Color color, Piece piece) {
    return _pieces[color][piece];
}