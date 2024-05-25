#include <Core/Position.hpp>

namespace Chess {
    class Logic {
        Core::Position position;
    public:
        void Move(Types::Square from, Types::Square to);
        std::pair<Types::Color, Types::Piece> GetPiece(Types::Square square);
        Types::Color PlayerNow();
    };
}
