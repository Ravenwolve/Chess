#include <Logic/Logic.hpp>

std::pair<Chess::Types::Color, Chess::Types::Piece> Chess::Logic::GetPiece(Chess::Types::Square square) {
    return position.GetPiece(square);
}

void Chess::Logic::Move(Chess::Types::Square from, Chess::Types::Square to) {
    if (PlayerNow()) {
        std::pair<Types::Color, Types::Piece> c_p_from = GetPiece(from);
        if (c_p_from.first == PlayerNow()) {
            position.GenerateMovesForPiece(c_p_from.second, from);
            if (position.InMoves(to)) {
                std::pair<Types::Color, Types::Piece> c_p_to = GetPiece(to);
                position.Apply({from, to, c_p_from.second, c_p_to.second, c_p_from.first, c_p_to.first});
            }
        }
    }
}

Chess::Types::Color Chess::Logic::PlayerNow() {
    return position.PlayerNow();
}