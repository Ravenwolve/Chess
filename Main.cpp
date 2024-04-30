#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>

namespace Chess
{
    class Bitboard
    {
    private:
        uint64_t bitboard;
    public:
        Bitboard(uint64_t value) {
            bitboard = value;
        }

        bool operator[](const uint8_t bitNumber) const {
            return (bitboard & (1ULL << bitNumber));
        }
        
        template <bool bitValue>
        void Set(const uint8_t bitNumber) {
            if constexpr (bitValue)
                bitboard |= 1ULL << bitNumber;
            else bitboard &= ~(1ULL << bitNumber);
        }

        int Count() const {
            return std::popcount(bitboard);
        }
    };
}

int main()
{
    Chess::Bitboard a = 5;
    std::cout << a.CountOf0()
    
}