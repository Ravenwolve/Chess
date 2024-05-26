#include <Logic/Logic.hpp>

std::pair<Chess::Types::Color, Chess::Types::Piece> Chess::Logic::GetPiece(Chess::Types::Square square) {
    return position.GetPiece(square);
}

void Chess::Logic::Move(Chess::Types::Square from, Chess::Types::Square to) {
    std::pair c_p_from = GetPiece(from);
    if (c_p_from.first == PlayerNow()) {
        position.GenerateMovesForPiece(c_p_from.second, from);
        if (position.InMoves(to)) {
            std::pair c_p_to = to == position.GetEnPassant() ? std::pair(static_cast<Types::Color>(!c_p_from.first), Types::Piece::Pawn) : GetPiece(to);
            position.Apply({from, to, c_p_from.second, c_p_to.second, c_p_from.first, c_p_to.first});
        }
    }
}

Chess::Types::Color Chess::Logic::PlayerNow() {
    return position.PlayerNow();
}

bool Chess::Logic::IsCheck() {
    return position.IsCheck();
}

bool Chess::Logic::IsCheckmate() {
    return IsStalemate() & IsCheck();
}

bool Chess::Logic::IsStalemate() {
    return position.NoMoves();
}