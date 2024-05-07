#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <bitset>
#include <sstream>
#include <tuple>

namespace Chess {
    using Bitboard = uint64_t;
    using Castling = std::bitset<4>;
    using EnPassant = Bitboard;
    using MovesWithoutCapturing = uint8_t;
    using NumberOfMove = uint8_t;
    using ShortFEN = std::string;

    enum Color {
        White, Black
    };

    enum Piece {
        Pawn, Rook, Knight, Bishop, Queen, King
    };

    bool Get(const Bitboard& obj, const uint8_t bitNumber);
    
    template <bool bitValue>
    void Set(Bitboard& obj, const uint8_t bitNumber);

    std::tuple<ShortFEN, Color, Castling, EnPassant, MovesWithoutCapturing, NumberOfMove> ParseFEN(const std::string& FEN);
    
    class PieceBitboardContainer {
    private:
        std::array<std::array<Bitboard, 6>, 2> _pieces;

        std::array<Bitboard, 2> _sides;
        Bitboard _union;

        void UpdateBitboards() noexcept;
    public:
        PieceBitboardContainer() noexcept;
        PieceBitboardContainer(const PieceBitboardContainer& other) noexcept;
        PieceBitboardContainer(const std::string& shortFEN);
    };

    class Mask;
}