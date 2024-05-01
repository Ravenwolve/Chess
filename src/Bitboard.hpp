#include <cstdint>

namespace Chess {
    class Bitboard {
    private:
        uint64_t bitboard;
        
    public:
        Bitboard(uint64_t value);
        Bitboard(const Bitboard& other);
        bool operator[](const uint8_t bitNumber) const;
        Bitboard operator|(const Bitboard& other) const noexcept;
        Bitboard operator&(const Bitboard& other) const noexcept;
        template <bool bitValue>
        void Set(const uint8_t bitNumber);
        constexpr int Count() const noexcept;
    };
}