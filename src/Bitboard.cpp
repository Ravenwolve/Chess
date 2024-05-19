#include "Bitboard.hpp"
#include <memory>
#include <vector>

// There is issue with bitNumber > 63 because of checking disabled for better performance
bool Chess::Core::Get(const Bitboard& obj, const uint8_t bitNumber) {
    return obj & (1ULL << bitNumber);
}

template <bool bitValue>
void Chess::Core::Set(Bitboard& obj, const uint8_t bitNumber) {
    if constexpr (bitValue)
        obj |= 1ULL << bitNumber;
    else obj &= ~(1ULL << bitNumber);
}

// --

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

void Chess::Core::BoardRepresentation::UpdateBitboards() noexcept {
    _sides[Black] = _sides[White] = 0;
    for (Bitboard item : _pieces[White])
        _sides[White] |= item;
    for (Bitboard item : _pieces[Black])
        _sides[Black] |= item;

    _unionAll = _sides[White] | _sides[Black];
}

std::tuple<Chess::Core::ShortFEN, Chess::Color, Chess::Core::Castling, Chess::Core::EnPassant, Chess::Core::MovesWithoutCapturing, Chess::Core::NumberOfMove>
Chess::Core::ParseFEN(const std::string &FEN) {
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

namespace Chess::Core::Cache {
    static constexpr std::array<std::array<Bitboard, 16>, 2> straightRays = {
        // -> =
        std::array<Bitboard, 16> {
            0b11111111ULL,
            0b11111111ULL << 8,
            0b11111111ULL << 16,
            0b11111111ULL << 24,
            0b11111111ULL << 32,
            0b11111111ULL << 40,
            0b11111111ULL << 48,
            0b11111111ULL << 56
        },
        // -> ||
        std::array<Bitboard, 16> {
            1ULL   | (1ULL << 8)   | (1ULL << 16)   | (1ULL << 24)   | (1ULL << 32)   | (1ULL << 40)   | (1ULL << 48)   | (1ULL << 56),
            2ULL   | (2ULL << 8)   | (2ULL << 16)   | (2ULL << 24)   | (2ULL << 32)   | (2ULL << 40)   | (2ULL << 48)   | (2ULL << 56),
            4ULL   | (4ULL << 8)   | (4ULL << 16)   | (4ULL << 24)   | (4ULL << 32)   | (4ULL << 40)   | (4ULL << 48)   | (4ULL << 56),
            8ULL   | (8ULL << 8)   | (8ULL << 16)   | (8ULL << 24)   | (8ULL << 32)   | (8ULL << 40)   | (8ULL << 48)   | (8ULL << 56),
            16ULL  | (16ULL << 8)  | (16ULL << 16)  | (16ULL << 24)  | (16ULL << 32)  | (16ULL << 40)  | (16ULL << 48)  | (16ULL << 56),
            32ULL  | (32ULL << 8)  | (32ULL << 16)  | (32ULL << 24)  | (32ULL << 32)  | (32ULL << 40)  | (32ULL << 48)  | (32ULL << 56),
            64ULL  | (64ULL << 8)  | (64ULL << 16)  | (64ULL << 24)  | (64ULL << 32)  | (64ULL << 40)  | (64ULL << 48)  | (64ULL << 56),
            128ULL | (128ULL << 8) | (128ULL << 16) | (128ULL << 24) | (128ULL << 32) | (128ULL << 40) | (128ULL << 48) | (128ULL << 56)
        }
    };

    static constexpr std::array<std::array<Bitboard, 15>, 2> diagonalRays = {
        // -> //
        std::array<Bitboard, 15> {
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
                                                                                                                                         1ULL << 56
        },
        // -> \\.
        std::array<Bitboard, 15> {
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
        }
    };

    consteval static std::array<Bitboard, 64> CalculateForRook() {
        std::array<Bitboard, 64> masks;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;

        for (uint8_t i = 0U; i < masks.size(); ++i)
            masks[i] = (~(1ULL << i) & (straightRays[0][i / 8] | straightRays[1][i % 8])) &
            ~((i > 8 ? straightRays[0][0] : 0ULL) | (i < 56 ? straightRays[0][7] : 0ULL) |
            (i % 8 ? straightRays[1][0] : 0ULL) | ((i + 1) % 8 ? straightRays[1][7] : 0ULL));

        return masks;
    }

    consteval static std::array<Bitboard, 64> CalculateForBishop() {
        std::array<Bitboard, 64> masks;
        Bitboard sourceBit;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;
        const Bitboard notFrame = ~(straightRays[0][0] | straightRays[0][7] | straightRays[1][0] | straightRays[1][7]);

        for (uint8_t i = 0U; i < masks.size(); ++i) {
            sourceBit = 1ULL << i;
            masks[i] = 0;
            for (uint8_t j = 0U; j < 15U; ++j)
                if (sourceBit & diagonalRays[0][j]) {
                    masks[i] |= ~sourceBit & diagonalRays[0][j];
                    break;
                }
            for (uint8_t j = 0U; j < 15U; ++j)
                if (sourceBit & diagonalRays[1][j]) {
                    masks[i] |= ~sourceBit & diagonalRays[1][j];
                    break;
                }
            masks[i] &= notFrame;
        }
        return masks;
    }

    //static constexpr std::array<Bitboard, 64> knightMasks = CalculateForKnight();
    static constexpr std::array<Bitboard, 64> rookMasks = CalculateForRook();
    static constexpr std::array<Bitboard, 64> bishopMasks = CalculateForBishop();
    //static constexpr std::array<Bitboard, 64> kingMasks = CalculateForKing();
    //static constexpr std::array<Bitboard, 64> pawnMasks = Chess::Mask::CalculateForPawn();

    consteval static std::array<Bitboard, 64> CalculateQueenMasks() {
        std::array<Bitboard, 64> masks;

        for (uint8_t i = 0; i < 64; ++i)
            masks[i] = rookMasks[i] | bishopMasks[i];

        return masks;
    }

    static constexpr std::array<Bitboard, 64> queenMasks = CalculateQueenMasks();
};

namespace Chess::Core::BitScan {
    constexpr std::array<uint8_t, 64> bitScanIndex {
        0, 47, 1, 56, 48, 27, 2, 60,
        57, 49, 41, 37, 28, 16, 3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11, 4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30, 9, 24,
        13, 18, 8, 12, 7, 6, 5, 63
    };

    /**
     * @author Kim Walisch (2012)
     * @return index (0..63) of least significant one bit
     */
    // For search of the least significant bit
    constexpr uint8_t Forward(Bitboard bitboard) {
        const Bitboard debruijn64 = 0x03f79d71b4cb0a89ULL;
        return bitScanIndex[((bitboard ^ (bitboard - 1)) * debruijn64) >> 58];
    }

    /**
     * @authors Kim Walisch, Mark Dickinson
     * @return index (0..63) of most significant one bit
     */
    // For search of the most significant bit
    constexpr uint8_t Reverse(Bitboard bitboard) {
        const Bitboard debruijn64 = 0x03f79d71b4cb0a89ULL;
        bitboard |= bitboard >> 1;
        bitboard |= bitboard >> 2;
        bitboard |= bitboard >> 4;
        bitboard |= bitboard >> 8;
        bitboard |= bitboard >> 16;
        bitboard |= bitboard >> 32;
        return bitScanIndex[(bitboard * debruijn64) >> 58];
    }
}

namespace Chess::Core::Cache {
    
    const std::array<uint8_t, 64> rookRelevantBits {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    };

    const std::array<uint8_t, 64> bishopRelevantBits {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
    };

    /** Rook magic numbers
    * @authors maksimKorzh
    */
    const std::array<uint64_t, 64> rookMagics {
        0xa8002c000108020ULL,
        0x6c00049b0002001ULL,
        0x100200010090040ULL,
        0x2480041000800801ULL,
        0x280028004000800ULL,
        0x900410008040022ULL,
        0x280020001001080ULL,
        0x2880002041000080ULL,
        0xa000800080400034ULL,
        0x4808020004000ULL,
        0x2290802004801000ULL,
        0x411000d00100020ULL,
        0x402800800040080ULL,
        0xb000401004208ULL,
        0x2409000100040200ULL,
        0x1002100004082ULL,
        0x22878001e24000ULL,
        0x1090810021004010ULL,
        0x801030040200012ULL,
        0x500808008001000ULL,
        0xa08018014000880ULL,
        0x8000808004000200ULL,
        0x201008080010200ULL,
        0x801020000441091ULL,
        0x800080204005ULL,
        0x1040200040100048ULL,
        0x120200402082ULL,
        0xd14880480100080ULL,
        0x12040280080080ULL,
        0x100040080020080ULL,
        0x9020010080800200ULL,
        0x813241200148449ULL,
        0x491604001800080ULL,
        0x100401000402001ULL,
        0x4820010021001040ULL,
        0x400402202000812ULL,
        0x209009005000802ULL,
        0x810800601800400ULL,
        0x4301083214000150ULL,
        0x204026458e001401ULL,
        0x40204000808000ULL,
        0x8001008040010020ULL,
        0x8410820820420010ULL,
        0x1003001000090020ULL,
        0x804040008008080ULL,
        0x12000810020004ULL,
        0x1000100200040208ULL,
        0x430000a044020001ULL,
        0x280009023410300ULL,
        0xe0100040002240ULL,
        0x200100401700ULL,
        0x2244100408008080ULL,
        0x8000400801980ULL,
        0x2000810040200ULL,
        0x8010100228810400ULL,
        0x2000009044210200ULL,
        0x4080008040102101ULL,
        0x40002080411d01ULL,
        0x2005524060000901ULL,
        0x502001008400422ULL,
        0x489a000810200402ULL,
        0x1004400080a13ULL,
        0x4000011008020084ULL,
        0x26002114058042ULL,
    };

    /** Bishop magic numbers
    * @authors maksimKorzh
    */
    const std::array<uint64_t, 64> bishopMagics {
        0x89a1121896040240ULL,
        0x2004844802002010ULL,
        0x2068080051921000ULL,
        0x62880a0220200808ULL,
        0x4042004000000ULL,
        0x100822020200011ULL,
        0xc00444222012000aULL,
        0x28808801216001ULL,
        0x400492088408100ULL,
        0x201c401040c0084ULL,
        0x840800910a0010ULL,
        0x82080240060ULL,
        0x2000840504006000ULL,
        0x30010c4108405004ULL,
        0x1008005410080802ULL,
        0x8144042209100900ULL,
        0x208081020014400ULL,
        0x4800201208ca00ULL,
        0xf18140408012008ULL,
        0x1004002802102001ULL,
        0x841000820080811ULL,
        0x40200200a42008ULL,
        0x800054042000ULL,
        0x88010400410c9000ULL,
        0x520040470104290ULL,
        0x1004040051500081ULL,
        0x2002081833080021ULL,
        0x400c00c010142ULL,
        0x941408200c002000ULL,
        0x658810000806011ULL,
        0x188071040440a00ULL,
        0x4800404002011c00ULL,
        0x104442040404200ULL,
        0x511080202091021ULL,
        0x4022401120400ULL,
        0x80c0040400080120ULL,
        0x8040010040820802ULL,
        0x480810700020090ULL,
        0x102008e00040242ULL,
        0x809005202050100ULL,
        0x8002024220104080ULL,
        0x431008804142000ULL,
        0x19001802081400ULL,
        0x200014208040080ULL,
        0x3308082008200100ULL,
        0x41010500040c020ULL,
        0x4012020c04210308ULL,
        0x208220a202004080ULL,
        0x111040120082000ULL,
        0x6803040141280a00ULL,
        0x2101004202410000ULL,
        0x8200000041108022ULL,
        0x21082088000ULL,
        0x2410204010040ULL,
        0x40100400809000ULL,
        0x822088220820214ULL,
        0x40808090012004ULL,
        0x910224040218c9ULL,
        0x402814422015008ULL,
        0x90014004842410ULL,
        0x1000042304105ULL,
        0x10008830412a00ULL,
        0x2520081090008908ULL,
        0x40102000a0a60140ULL,
    };

    std::array<std::vector<Bitboard>, 64> rookCache; //[64][4096];
    std::array<std::vector<Bitboard>, 64> bishopCache; //[64][512];

    uint8_t Hash(uint8_t square, Bitboard occupancy, Piece piece) {
        if (piece == Rook)
            return (occupancy * rookMagics[square]) >> (64 - rookRelevantBits[square]);
        else return (occupancy * bishopMagics[square]) >> (64 - bishopRelevantBits[square]);
    }

    Bitboard FindOccupancy(uint16_t index, Bitboard primaryMoveMask, uint8_t countOfInterestingSquares) {
        Bitboard occupancy = 0ULL;
        // loop over the range of possible squares within primary move mask (without blocking pieces)
        for (uint8_t i = 0, currentSquare; i < countOfInterestingSquares; ++i) {
            // get LS1B index of attacks mask
            currentSquare = BitScan::Forward(primaryMoveMask);
            // pop LS1B in attack map
            primaryMoveMask &= ~(1ULL << currentSquare);
            // make sure occupancy is on board
            if (index & (1ULL << i))
                // populate occupancy map
                occupancy |= (1ULL << currentSquare);
        }
        return occupancy;
    }

    Bitboard GetRookSlidingAttacks(uint8_t square, Bitboard blockingPieces) {
        const int8_t targetFile = square % 8;
        const int8_t targetRank = square - targetFile;
        const uint8_t _1RANK = 8,_1FILE = 1;
        Bitboard attack = 0ULL, attacked;
        
        Bitboard shifted = 1ULL << targetRank;
        for (int8_t file = targetFile - _1FILE; file >= File::A; --file) {
            attacked = shifted << file;
            attack |= attacked;
            if (blockingPieces & attacked)
                break;
        }
        for (int8_t file = targetFile + _1FILE; file <= File::H; ++file) {
            attacked = shifted << file;
            attack |= attacked;
            if (blockingPieces & attacked)
                break;
        }

        shifted = 1ULL << targetFile;
        for (int8_t rank = targetRank - _1RANK; rank >= Rank::_1; rank -= _1RANK) {
            attacked = shifted << rank;
            attack |= attacked;
            if (blockingPieces & attacked)
                break;
        }
        for (int8_t rank = targetRank + _1RANK; rank <= Rank::_8; rank += _1RANK) {
            attacked = shifted << rank;
            attack |= attacked;
            if (blockingPieces & attacked)
                break;
        }
        return attack;
    }

    Bitboard GetBishopSlidingAttacks(uint8_t square, Bitboard blockingPieces) {
        const int8_t targetFile = square % 8;
        const int8_t targetRank = square - targetFile;
        const uint8_t _1RANK = 8,_1FILE = 1;
        Bitboard attack = 0ULL, attacked;

        for (int8_t file = targetFile - _1FILE, rank = targetRank - _1RANK; file >= File::A && rank >= Rank::_1; --file, rank -= _1RANK) {
            attacked = (1ULL << file) << rank;
            attack |= attacked;
            if (blockingPieces & attacked)
                break;
        }
        for (int8_t file = targetFile + _1FILE, rank = targetRank + _1RANK; file <= File::H && rank <= Rank::_8; ++file, rank += _1RANK) {
            attacked = (1ULL << file) << rank;
            attack |= attacked;
            if (blockingPieces & attacked)
                break;
        }
        for (int8_t file = targetFile - _1FILE, rank = targetRank + _1RANK; file >= File::A && rank <= Rank::_8; --file, rank += _1RANK) {
            attacked = (1ULL << file) << rank;
            attack |= attacked;
            if (blockingPieces & attacked)
                break;
        }
        for (int8_t file = targetFile + _1FILE, rank = targetRank - _1RANK; file <= File::H && rank >= Rank::_1; ++file, rank -= _1RANK) {
            attacked = (1ULL << file) << rank;
            attack |= attacked;
            if (blockingPieces & attacked)
                break;
        }
        return attack;
    }

    void InitCache() {
        Bitboard occupancy;
        uint8_t count;
        uint16_t hash, variants;
        for (uint8_t square = 0; square < 64; ++square) {
            count = rookRelevantBits[square];
            variants = 1 << count;
            rookCache[square].reserve(variants);
            for (uint16_t index = 0; index < variants; ++index) {
                occupancy = FindOccupancy(index, rookMasks[square], count);
                hash = Hash(square, occupancy, Rook);
                rookCache[square][hash] = GetRookSlidingAttacks(square, occupancy);
            }

            count = bishopRelevantBits[square];
            variants = 1 << count;
            bishopCache[square].reserve(variants);
            for (uint16_t index = 0; index < variants; ++index) {
                occupancy = FindOccupancy(index, bishopMasks[square], count);
                hash = Hash(square, occupancy, Bishop);
                bishopCache[square][hash] = GetBishopSlidingAttacks(square, occupancy);
            }
        }
    }
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetPawnAttacks(Chess::Color color, Chess::Core::Bitboard bitboard) noexcept {
    if (color == White)
        return (bitboard << 7) & ~Cache::straightRays[1][0] | (bitboard << 9) & ~Cache::straightRays[1][7];
    else return (bitboard >> 7) & ~Cache::straightRays[1][0] | (bitboard >> 9) & ~Cache::straightRays[1][7];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetPawnAdvances(Chess::Color color, Chess::Core::Bitboard bitboard) noexcept {
    if (color == White)
        return (bitboard << 8 | ((bitboard & Cache::straightRays[0][1]) << 16));
    else return ((bitboard >> 8) | ((bitboard & Cache::straightRays[0][6]) >> 16));
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetKnightAttacks(Chess::Core::Bitboard bitboard) noexcept {
    Bitboard lr1 = bitboard >> 1 & ~Cache::straightRays[1][7] | bitboard << 1 & ~Cache::straightRays[1][0];
    Bitboard lr2 = bitboard >> 2 & ~(Cache::straightRays[1][6] | Cache::straightRays[1][7]) |
                   bitboard << 2 & ~(Cache::straightRays[1][0] | Cache::straightRays[1][1]);
    return lr1 << 16 | lr1 >> 16 | lr2 << 8 | lr2 >> 8;
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetKingAttacks(Chess::Core::Bitboard bitboard) noexcept {
    Bitboard lr1 = bitboard >> 1 & ~Cache::straightRays[1][7] | bitboard << 1 & ~Cache::straightRays[1][0];
    return lr1 | lr1 << 8 | lr1 >> 8 | bitboard << 8 | bitboard >> 8;
}

/**
    Magic bitboards method for sliding pieces
    @link (@https://www.chessprogramming.org/Magic_Bitboards) for description
*/
Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetRookAttacks(uint8_t square) {
    Bitboard occupancy = _unionAll & Cache::rookMasks[square];
    return Cache::rookCache[square][Cache::Hash(square, occupancy, Rook)];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetBishopAttacks(uint8_t square) {
    Bitboard occupancy = _unionAll & Cache::bishopMasks[square];
    return Cache::bishopCache[square][Cache::Hash(square, occupancy, Bishop)];
}