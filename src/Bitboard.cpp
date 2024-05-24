#include <Bitboard.hpp>
#include <memory>
#include <vector>
#include <BitScan.hpp>

using enum Chess::Types::Square;
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

namespace Chess::Core::Cache {
    static constexpr std::array<Bitboard, 8> ranks {
        0b11111111ULL,
        0b11111111ULL << 8,
        0b11111111ULL << 16,
        0b11111111ULL << 24,
        0b11111111ULL << 32,
        0b11111111ULL << 40,
        0b11111111ULL << 48,
        0b11111111ULL << 56
    };
    static constexpr std::array<Bitboard, 8> files {
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

    static consteval std::array<Bitboard, 64> CalculateRaysForRook() {
        std::array<Bitboard, 64> masks;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;

        for (uint8_t i = 0U; i < masks.size(); ++i)
            masks[i] = (~(1ULL << i) & (ranks[i / 8] | files[i % 8])) &
            ~((i > 8 ? ranks[0] : 0ULL) | (i < 56 ? ranks[7] : 0ULL) |
            (i % 8 ? files[File::A] : 0ULL) | ((i + 1) % 8 ? files[File::H] : 0ULL));

        return masks;
    }

    static consteval std::array<Bitboard, 64> CalculateRaysForBishop() {
        std::array<Bitboard, 64> masks;
        Bitboard sourceBit;
        const int8_t UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1;
        const Bitboard notFrame = ~(ranks[0] | ranks[7] | files[File::A] | files[File::H]);

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

    static consteval std::array<Bitboard, 64> CalculateForKnight() {
        std::array<Bitboard, 64> masks;
        Bitboard tmp, lr1, lr2;
        for (uint8_t sq = 0; sq < 64; ++sq) {
            tmp = 1ULL << sq;
            lr1 = tmp >> 1 & ~files[File::H] | tmp << 1 & ~files[File::A];
            lr2 = tmp >> 2 & ~(files[File::G] | files[File::H]) |
                       tmp << 2 & ~(files[File::A] | files[File::B]);
            masks[sq] = lr1 << 16 | lr1 >> 16 | lr2 << 8 | lr2 >> 8;
        }
        return masks;
    }

    static consteval std::array<Bitboard, 64> CalculateForKing() {
        std::array<Bitboard, 64> masks;
        Bitboard tmp, lr;
        for (uint8_t sq = 0; sq < 64; ++sq) {
            tmp = 1ULL << sq;
            lr = tmp >> 1 & ~files[File::H] | tmp << 1 & ~files[File::A];
            masks[sq] = lr | lr << 8 | lr >> 8 | tmp << 8 | tmp >> 8;
        }
        return masks;
    }

    static constexpr std::array<Bitboard, 64> knightMasks = CalculateForKnight();
    static constexpr std::array<Bitboard, 64> rookMasks = CalculateRaysForRook();
    static constexpr std::array<Bitboard, 64> bishopMasks = CalculateRaysForBishop();
    static constexpr std::array<Bitboard, 64> kingMasks = CalculateForKing();
};

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

    std::unique_ptr rookCache = std::make_unique<std::array<std::vector<Bitboard>, 64>>();
    std::unique_ptr bishopCache = std::make_unique<std::array<std::vector<Bitboard>, 64>>();

    uint8_t Hash(Square square, Bitboard occupancy, Piece piece) {
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

    Bitboard GetRookSlidingAttacks(Square square, Bitboard blockingPieces) {
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

    Bitboard GetBishopSlidingAttacks(Square square, Bitboard blockingPieces) {
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
            (*rookCache)[square].reserve(variants);
            for (uint16_t index = 0; index < variants; ++index) {
                occupancy = FindOccupancy(index, rookMasks[square], count);
                hash = Hash(static_cast<Square>(square), occupancy, Rook);
                (*rookCache)[square][hash] = GetRookSlidingAttacks(static_cast<Square>(square), occupancy);
            }

            count = bishopRelevantBits[square];
            variants = 1 << count;
            (*bishopCache)[square].reserve(variants);
            for (uint16_t index = 0; index < variants; ++index) {
                occupancy = FindOccupancy(index, bishopMasks[square], count);
                hash = Hash(static_cast<Square>(square), occupancy, Bishop);
                (*bishopCache)[square][hash] = GetBishopSlidingAttacks(static_cast<Square>(square), occupancy);
            }
        }
    }
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetPawnAttacks(Color color, Square square) noexcept {
    if (color == White)
        return ((1ULL << square) << 7) & ~Cache::files[File::H] | ((1ULL << square) << 9) & ~Cache::files[File::A] & _sides[Black];
    else return ((1ULL << square) >> 7) & ~Cache::files[File::H] | ((1ULL << square) >> 9) & ~Cache::files[File::A] & _sides[White];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetPawnAdvances(Color color, Square square) noexcept {
    Bitboard advance = (color == White ? (1ULL << square) << 8 : ((1ULL << square) >> 8)) & ~_unionAll; // Short
    advance |= (color == White ? (advance & Rank::_3) << 8 : ((advance & Rank::_6) >> 8)) & ~_unionAll; // Long
    return advance;
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetKnightAttacks(Color color, Square square) noexcept {
    return Cache::knightMasks[square] & ~_sides[color];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetKingAttacks(Color color, Square square) noexcept {
    return Cache::kingMasks[square] & ~_sides[color];
}

/**
    Magic bitboards method for sliding pieces
    @link (@https://www.chessprogramming.org/Magic_Bitboards) for description
*/
Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetRookAttacks(Color color, Square square) {
    return (*Cache::rookCache)[square][Cache::Hash(square, _unionAll & Cache::rookMasks[square], Rook)] & ~_sides[color];
}

Chess::Core::Bitboard Chess::Core::BoardRepresentation::GetBishopAttacks(Color color, Square square) {
    return (*Cache::bishopCache)[square][Cache::Hash(square, _unionAll & Cache::bishopMasks[square], Bishop)] & ~_sides[color];
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