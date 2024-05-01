#include <bit>
#include "Bitboard.hpp"

namespace Chess {    
    Bitboard::Bitboard(uint64_t value) {
        bitboard = value;
    }

    Bitboard::Bitboard(const Bitboard& other) {
        this->bitboard = other.bitboard;
    }

    bool Bitboard::operator[](const uint8_t bitNumber) const {
        return (bitboard & (1ULL << bitNumber));
    }

    Bitboard Bitboard::operator|(const Bitboard& other) const noexcept {
        return Bitboard(this->bitboard | other.bitboard);
    }

    Bitboard Bitboard::operator&(const Bitboard& other) const noexcept {
        return Bitboard(this->bitboard & other.bitboard);
    }
        
    template <bool bitValue>
    void Bitboard::Set(const uint8_t bitNumber) {
        if constexpr (bitValue)
            bitboard |= 1ULL << bitNumber;
        else bitboard &= ~(1ULL << bitNumber);
    }

    constexpr int Bitboard::Count() const noexcept {
        return std::popcount(bitboard);
    }
}