#include <bit>
#include "Bitboard.hpp"

// There is issue with bitNumber > 63 because of checking disabled for better performance
bool Chess::Get(const Bitboard& obj, const uint8_t bitNumber) {
    return obj & (1ULL << bitNumber);
}

template <bool bitValue>
void Chess::Set(Bitboard& obj, const uint8_t bitNumber) {
    if constexpr (bitValue)
        obj |= 1ULL << bitNumber;
    else obj &= ~(1ULL << bitNumber);
}

// --

Chess::PieceBitboardContainer::PieceBitboardContainer() noexcept {
    _pieces[White] = {
        Bitboard(0b11111111) << 8, // Pawns / FEN: P
        Bitboard(0b10000001),      // Rooks / FEN: R
        Bitboard(0b01000010),      // Knights / FEN: N
        Bitboard(0b00100100),      // Bishops / FEN: B
        Bitboard(0b00010000),      // Queen / FEN: Q
        Bitboard(0b00001000)      // King / FEN: K
    };
    // Black
    _pieces[Black] = {
        Bitboard(0b11111111) << 48, // Pawns / FEN: p
        Bitboard(0b10000001) << 56, // Rooks / FEN: r
        Bitboard(0b01000010) << 56, // Knights / FEN: n
        Bitboard(0b00100100) << 56, // Bishops / FEN: b
        Bitboard(0b00010000) << 56, // Queen / FEN: q
        Bitboard(0b00001000) << 56 // King / FEN: k
    };

    UpdateBitboards();
}

Chess::PieceBitboardContainer::PieceBitboardContainer(const PieceBitboardContainer &other) noexcept = default;

Chess::PieceBitboardContainer::PieceBitboardContainer(const std::string& shortFEN) {
    // default: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
    for (Bitboard& item : _pieces[White])
        item = 0;
    for (Bitboard& item : _pieces[Black])
        item = 0;

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

            switch (ch) {
                case 'r': _pieces[color][Rook] |= 1ULL << y * 8 + x; break;
                case 'n': _pieces[color][Knight] |= 1ULL << y * 8 + x; break;
                case 'b': _pieces[color][Bishop] |= 1ULL << y * 8 + x; break;
                case 'q': _pieces[color][Queen] |= 1ULL << y * 8 + x; break;
                case 'k': _pieces[color][King] |= 1ULL << y * 8 + x; break;
                case 'p': _pieces[color][Pawn] |= 1ULL << y * 8 + x; break;
            }

            ++x;
        }
    }

    UpdateBitboards();
}

void Chess::PieceBitboardContainer::UpdateBitboards() noexcept {
    _sides[Black] = _sides[White] = 0;
    for (Bitboard item : _pieces[White])
        _sides[White] |= item;
    for (Bitboard item : _pieces[Black])
        _sides[Black] |= item;

    _union = _sides[White] | _sides[Black];
}

std::tuple<Chess::ShortFEN, Chess::Color, Chess::Castling, Chess::EnPassant, Chess::MovesWithoutCapturing, Chess::NumberOfMove>
Chess::ParseFEN(const std::string &FEN) {
    std::string splittedFEN[4];
    MovesWithoutCapturing count;
    NumberOfMove number;
    std::stringstream ss(FEN);

    for (size_t i = 0; i < 4; ++i)
        ss >> splittedFEN[i];
    ss >> count >> number;

    Castling castling;
    for (char ch : splittedFEN[2])
    {
        switch (ch)
        {
        case 'K':
            castling.set(0);
            break;
        case 'Q':
            castling.set(1);
            break;
        case 'k':
            castling.set(2);
            break;
        case 'q':
            castling.set(3);
            break;
        }
    }

    return std::make_tuple(
        splittedFEN[0],
        splittedFEN[1][0] == 'w' ? White : Black,
        castling,
        splittedFEN[3][0] != '-' ? (size_t)(splittedFEN[3][0] - 'a') + (size_t)(splittedFEN[3][1] - '0') * 8 : 0,
        count,
        number
        );

    // size_t index = FEN.find(' ');
    // ShortFEN shortFEN = FEN.substr(0, index);
    // Color color = FEN[index + 1] == 'w' ? White : Black;
    // Castling castling;
    // for (index += 3; FEN[index] != ' '; ++index) {
    //     switch (FEN[index]) {
    //         case 'K': castling.set(0); break;
    //         case 'Q': castling.set(1); break;
    //         case 'k': castling.set(2); break;
    //         case 'q': castling.set(3); break;
    //     }
    // }
    // EnPassant enPassant = 0;
    // if (FEN[index + 1] != '-') {
    //     enPassant = (size_t)(FEN[index + 1] - 'a') + (size_t)(FEN[index + 2] - '0') * 8;
    //     ++index;
    // }
    // std::string twoNumbers = FEN.substr(index + 3, FEN.length() - (index + 3));
}