#include <CLI/Image.hpp>

std::array<std::array<std::string, 6>, 2> Chess::UI::Image::pieces {
    std::array<std::string, 6> {
        "\033[1;37mP\033[0m",
        "\033[1;37mR\033[0m",
        "\033[1;37mN\033[0m",
        "\033[1;37mB\033[0m",
        "\033[1;37mQ\033[0m",
        "\033[1;37mK\033[0m",
    },
    std::array<std::string, 6> {
        "\033[1;31mp\033[0m",
        "\033[1;31mr\033[0m",
        "\033[1;31mn\033[0m",
        "\033[1;31mb\033[0m",
        "\033[1;31mq\033[0m",
        "\033[1;31mk\033[0m",
    }
};

std::string Chess::UI::Image::Get(Types::Color color, Types::Piece piece) {
    return pieces[color][piece];
}

