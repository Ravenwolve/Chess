#include "PieceBitboardContainer.hpp"

namespace Chess {
    PieceBitboardContainer::PieceBitboardContainer() {
        pieceBitboards = new std::array<std::array<Bitboard, 6>, 2>{
            // White
            std::array<Bitboard, 6>({
                Bitboard(uint64_t(0b11111111) << 8), // Pawns / FEN: P
                Bitboard(uint64_t(0b10000001)), // Rooks / FEN: R
                Bitboard(uint64_t(0b01000010)), // Knights / FEN: N
                Bitboard(uint64_t(0b00100100)), // Bishops / FEN: B
                Bitboard(uint64_t(0b00010000)), // Queen / FEN: Q
                Bitboard(uint64_t(0b00001000)), // King / FEN: K
            }),
            // Black
            std::array<Bitboard, 6>({
                Bitboard(uint64_t(0b11111111) << 48), // Pawns / FEN: p
                Bitboard(uint64_t(0b10000001) << 56), // Rooks / FEN: r
                Bitboard(uint64_t(0b01000010) << 56), // Knights / FEN: n
                Bitboard(uint64_t(0b00100100) << 56), // Bishops / FEN: b
                Bitboard(uint64_t(0b00010000) << 56), // Queen / FEN: q
                Bitboard(uint64_t(0b00001000) << 56), // King / FEN: k
            })
        };
    }
        // // Нужна ли в Init эта переменная?
        // std::array<Bitboard, 2> sideBitboards = {
        //     // White
        //     pieceBitboards[Color::White][Piece::Pawn] | pieceBitboards[Color::White][Piece::Rook] | pieceBitboards[Color::White][Piece::Knight]
        //     | pieceBitboards[Color::White][Piece::Bishop] | pieceBitboards[Color::White][Piece::Queen] | pieceBitboards[Color::White][Piece::King], // King ?
        //     pieceBitboards[Color::Black][Piece::Pawn] | pieceBitboards[Color::Black][Piece::Rook] | pieceBitboards[Color::Black][Piece::Knight]
        //     | pieceBitboards[Color::Black][Piece::Bishop] | pieceBitboards[Color::Black][Piece::Queen] | pieceBitboards[Color::Black][Piece::King], // King ?
        // };
}