#pragma once
#include <Core/Types.hpp>
#include <array>
#include <string>

namespace Chess::UI {
    class Image {
        static std::array<std::array<std::string, 6>, 2> pieces;
    public:
        static std::string Get(Types::Color color, Types::Piece piece);
    };
}