#pragma once
#include <Core/Types.hpp>

namespace Chess::Types {
    struct Move {
        Types::Square from, to;
        Types::Piece attackerType, targetType;
        Types::Color attackerColor, targetColor;
    };
}