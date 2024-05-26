#pragma once
#include <array>
#include <cstdint>
#include <Core/Calc.hpp>

namespace Chess::Core::Cache {
    static bool cacheInitialized = false;
    void InitCache();
    Bitboard GetSlidingMask(Types::Piece piece, Types::Square square);
    Bitboard GetCache(Types::Piece piece, Types::Square square, uint16_t index = 0);
    uint16_t Hash(Types::Square square, Bitboard occupancy, Types::Piece piece);
}