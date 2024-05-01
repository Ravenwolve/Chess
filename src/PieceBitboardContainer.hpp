#include <array>
#include "Bitboard.hpp"

namespace Chess {
    enum Color {
        White, Black
    };

    enum Piece {
        Pawn, Rook, Knight, Bishop, Queen, King
    };
    
    class PieceBitboardContainer {
    private:
        std::array<std::array<Bitboard, 6>, 2>* pieceBitboards;

    public:
        PieceBitboardContainer();
        PieceBitboardContainer(const PieceBitboardContainer& other);
    };
}