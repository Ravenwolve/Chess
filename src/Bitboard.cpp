#include <bit>
#include "../includes/Bitboard.hpp"

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
        0b11111111ULL << 8, // Pawns / FEN: P
        0b10000001ULL,      // Rooks / FEN: R
        0b01000010ULL,      // Knights / FEN: N
        0b00100100ULL,      // Bishops / FEN: B
        0b00001000ULL,      // Queen / FEN: Q
        0b00010000ULL      // King / FEN: K
    };
    // Black
    _pieces[Black] = {
        Bitboard(0b11111111) << 48, // Pawns / FEN: p
        Bitboard(0b10000001) << 56, // Rooks / FEN: r
        Bitboard(0b01000010) << 56, // Knights / FEN: n
        Bitboard(0b00100100) << 56, // Bishops / FEN: b
        Bitboard(0b00001000) << 56, // Queen / FEN: q
        Bitboard(0b00010000) << 56 // King / FEN: k
    };

    UpdateBitboards();
}

Chess::PieceBitboardContainer::PieceBitboardContainer(const PieceBitboardContainer &other) noexcept = default;

Chess::PieceBitboardContainer::PieceBitboardContainer(const std::string& shortFEN) {
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

    for (size_t i = 0UL; i < 4UL; ++i)
        ss >> splittedFEN[i];
    ss >> count >> number;

    Castling castling;
    for (char ch : splittedFEN[2]) {
        switch (ch) {
        case 'K': castling.set(0); break;
        case 'Q': castling.set(1); break;
        case 'k': castling.set(2); break;
        case 'q': castling.set(3); break;
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
}

namespace Chess::Mask {
    static constexpr std::array<std::array<Bitboard, 16>, 2> straightRays = {
        // -> =
        0b11111111ULL,
        0b11111111ULL << 8,
        0b11111111ULL << 16,
        0b11111111ULL << 24,
        0b11111111ULL << 32,
        0b11111111ULL << 40,
        0b11111111ULL << 48,
        0b11111111ULL << 56,
        // -> ||
        1ULL   | (1ULL << 8)   | (1ULL << 16)   | (1ULL << 24)   | (1ULL << 32)   | (1ULL << 40)   | (1ULL << 48)   | (1ULL << 56),
        2ULL   | (2ULL << 8)   | (2ULL << 16)   | (2ULL << 24)   | (2ULL << 32)   | (2ULL << 40)   | (2ULL << 48)   | (2ULL << 56),
        4ULL   | (4ULL << 8)   | (4ULL << 16)   | (4ULL << 24)   | (4ULL << 32)   | (4ULL << 40)   | (4ULL << 48)   | (4ULL << 56),
        8ULL   | (8ULL << 8)   | (8ULL << 16)   | (8ULL << 24)   | (8ULL << 32)   | (8ULL << 40)   | (8ULL << 48)   | (8ULL << 56),
        16ULL  | (16ULL << 8)  | (16ULL << 16)  | (16ULL << 24)  | (16ULL << 32)  | (16ULL << 40)  | (16ULL << 48)  | (16ULL << 56),
        32ULL  | (32ULL << 8)  | (32ULL << 16)  | (32ULL << 24)  | (32ULL << 32)  | (32ULL << 40)  | (32ULL << 48)  | (32ULL << 56),
        64ULL  | (64ULL << 8)  | (64ULL << 16)  | (64ULL << 24)  | (64ULL << 32)  | (64ULL << 40)  | (64ULL << 48)  | (64ULL << 56),
        128ULL | (128ULL << 8) | (128ULL << 16) | (128ULL << 24) | (128ULL << 32) | (128ULL << 40) | (128ULL << 48) | (128ULL << 56)
    };

    static constexpr std::array<std::array<Bitboard, 15>, 2> diagonalRays = {
        // -> //
        1ULL << 7,
        1ULL << 6 | 1ULL << (7 + 8),
        1ULL << 5 | 1ULL << (6 + 8) | 1ULL << (7 + 16),
        1ULL << 4 | 1ULL << (5 + 8) | 1ULL << (6 + 16) | 1ULL << (7 + 24),
        1ULL << 3 | 1ULL << (4 + 8) | 1ULL << (5 + 16) | 1ULL << (6 + 24) | 1ULL << (7 + 32),
        1ULL << 2 | 1ULL << (3 + 8) | 1ULL << (4 + 16) | 1ULL << (5 + 24) | 1ULL << (6 + 32) | 1ULL << (7 + 40),
        1ULL << 1 | 1ULL << (2 + 8) | 1ULL << (3 + 16) | 1ULL << (4 + 24) | 1ULL << (5 + 32) | 1ULL << (6 + 40) | 1ULL << (7 + 48),  
        1ULL      | 1ULL << (1 + 8) | 1ULL << (2 + 16) | 1ULL << (3 + 24) | 1ULL << (4 + 32) | 1ULL << (5 + 40) | 1ULL << (6 + 48) | 1ULL << (7 + 56),
                    1ULL << 8       | 1ULL << (1 + 16) | 1ULL << (2 + 24) | 1ULL << (3 + 32) | 1ULL << (4 + 40) | 1ULL << (5 + 48) | 1ULL << (6 + 56),
                                      1ULL << 16       | 1ULL << (1 + 24) | 1ULL << (2 + 32) | 1ULL << (3 + 40) | 1ULL << (4 + 48) | 1ULL << (5 + 56),
                                                         1ULL << 24       | 1ULL << (1 + 32) | 1ULL << (2 + 40) | 1ULL << (3 + 48) | 1ULL << (4 + 56),
                                                                            1ULL << 32       | 1ULL << (1 + 40) | 1ULL << (2 + 48) | 1ULL << (3 + 56),
                                                                                               1ULL << 40       | 1ULL << (1 + 48) | 1ULL << (2 + 56),
                                                                                                                  1ULL << 48       | 1ULL << (1 + 56),
                                                                                                                                     1ULL << 56,
        // -> \\.
        1ULL,
        1ULL << 1 | 1ULL << 8,
        1ULL << 2 | 1ULL << (1 + 8) | 1ULL << 16,
        1ULL << 3 | 1ULL << (2 + 8) | 1ULL << (1 + 16) | 1ULL << 24,
        1ULL << 4 | 1ULL << (3 + 8) | 1ULL << (2 + 16) | 1ULL << (1 + 24) | 1ULL << 32,
        1ULL << 5 | 1ULL << (4 + 8) | 1ULL << (3 + 16) | 1ULL << (2 + 24) | 1ULL << (1 + 32) | 1ULL << 40,
        1ULL << 6 | 1ULL << (5 + 8) | 1ULL << (4 + 16) | 1ULL << (3 + 24) | 1ULL << (2 + 32) | 1ULL << (1 + 40) | 1ULL << 48,  
        1ULL << 7 | 1ULL << (6 + 8) | 1ULL << (5 + 16) | 1ULL << (4 + 24) | 1ULL << (3 + 32) | 1ULL << (2 + 40) | 1ULL << (1 + 48) | 1ULL << 56,
                    1ULL << (7 + 8) | 1ULL << (6 + 16) | 1ULL << (5 + 24) | 1ULL << (4 + 32) | 1ULL << (3 + 40) | 1ULL << (2 + 48) | 1ULL << (1 + 56),
                                      1ULL << (7 + 16) | 1ULL << (6 + 24) | 1ULL << (5 + 32) | 1ULL << (4 + 40) | 1ULL << (3 + 48) | 1ULL << (2 + 56),
                                                         1ULL << (7 + 24) | 1ULL << (6 + 32) | 1ULL << (5 + 40) | 1ULL << (4 + 48) | 1ULL << (3 + 56),
                                                                            1ULL << (7 + 24) | 1ULL << (6 + 40) | 1ULL << (5 + 48) | 1ULL << (4 + 56),
                                                                                               1ULL << (7 + 40) | 1ULL << (6 + 48) | 1ULL << (5 + 56),
                                                                                                                  1ULL << (7 + 48) | 1ULL << (6 + 56),
                                                                                                                                     1ULL << (7 + 56)
    };

    consteval static std::array<Bitboard, 64> CalculateKnightMasks() {
        std::array<Bitboard, 64> masks;
        Bitboard sourceBit = 1;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;

        for (uint8_t i = 0U; i < masks.size(); ++i) {
            sourceBit = 1ULL << i;
            masks[i] = 0ULL;
            if (!(straightRays[1][0] | straightRays[0][6] | straightRays[0][7]) & sourceBit)
                masks[i] |= sourceBit << (UP + UP + LEFT);
            if (!(straightRays[1][7] | straightRays[0][6] | straightRays[0][7]) & sourceBit)
                masks[i] |= sourceBit << (UP + UP + RIGHT);
            if (!(straightRays[1][0] | straightRays[1][1] | straightRays[0][7]) & sourceBit)
                masks[i] |= sourceBit << (UP + LEFT + LEFT);
            if (!(straightRays[1][6] | straightRays[1][7] | straightRays[0][7]) & sourceBit)
                masks[i] |= sourceBit << (UP + RIGHT + RIGHT);
            if (!(straightRays[1][0] | straightRays[1][1] | straightRays[0][0]) & sourceBit)
                masks[i] |= sourceBit << (DOWN + LEFT + LEFT);
            if (!(straightRays[1][6] | straightRays[1][7] | straightRays[0][0]) & sourceBit)
                masks[i] |= sourceBit << (DOWN + RIGHT + RIGHT);
            if (!(straightRays[1][0] | straightRays[0][0] | straightRays[0][1]) & sourceBit)
                masks[i] |= sourceBit << (DOWN + DOWN + LEFT);
            if (!(straightRays[1][7] | straightRays[0][0] | straightRays[0][1]) & sourceBit)
                masks[i] |= sourceBit << (DOWN + DOWN + RIGHT);
        }
        return masks;
    }

    consteval static std::array<Bitboard, 64> CalculateKingMasks() {
        std::array<Bitboard, 64> masks;
        Bitboard sourceBit;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;

        for (uint8_t i = 0U; i < masks.size(); ++i) {
            sourceBit = 1ULL << i;
            masks[i] = 0ULL;
            if (!straightRays[0][0] & sourceBit) {
                masks[i] |= sourceBit << (DOWN);
                if (!straightRays[1][0] & sourceBit)
                    masks[i] |= (sourceBit << (DOWN + LEFT)) | (sourceBit << LEFT);
                if (!straightRays[1][7] & sourceBit)
                    masks[i] |= (sourceBit << (DOWN + RIGHT)) | (sourceBit << RIGHT);
            }
            if (!straightRays[0][7] & sourceBit) {
                masks[i] |= sourceBit << (UP);
                if (!straightRays[1][0] & sourceBit)
                    masks[i] |= (sourceBit << (UP + LEFT)) | (sourceBit << LEFT);
                if (!straightRays[1][7] & sourceBit)
                    masks[i] |= (sourceBit << (UP + RIGHT)) | (sourceBit << RIGHT);
            }
        }
        return masks;
    }

    consteval static std::array<Bitboard, 64> CalculateRookMasks() {
        std::array<Bitboard, 64> masks;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;

        for (uint8_t i = 0U; i < masks.size(); ++i)
            masks[i] = ~(1ULL << i) & (straightRays[0][i / 8] | straightRays[1][i % 8]);
        return masks;
    }

    consteval static std::array<Bitboard, 64> CalculateBishopMasks() {
        std::array<Bitboard, 64> masks;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;

        for (uint8_t i = 0U; i < masks.size(); ++i)
            masks[i] = ~(1ULL << i) & (diagonalRays[1][i / 8 + i % 8] | diagonalRays[0][i / 8]); // not ready
        return masks;
    }

    static constexpr std::array<Bitboard, 64> knightMasks = Chess::Mask::CalculateKnightMasks();
    static constexpr std::array<Bitboard, 64> rookMasks = Chess::Mask::CalculateRookMasks();
    static constexpr std::array<Bitboard, 64> bishopMasks = Chess::Mask::CalculateBishopMasks();
    static constexpr std::array<Bitboard, 64> kingMasks = Chess::Mask::CalculateKingMasks();
};