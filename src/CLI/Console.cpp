#include <CLI/Console.hpp>
#include <CLI/Image.hpp>
#include <iostream>

const std::string Chess::UI::CLI::line = "  +---+---+---+---+---+---+---+---+\n";
const std::string Chess::UI::CLI::files = "    A   B   C   D   E   F   G   H\n";

void Chess::UI::CLI::Refresh() {
    using namespace Types;
    std::string newBoard, row;
    std::pair<Color, Piece> c_p;
    for (uint8_t rank = 0U; rank < 8U; ++rank) {
        row = std::to_string(rank + 1) + " | ";
        for (uint8_t i = rank * 8U + 0U; i < rank * 8U + 8U; ++i) {
            c_p = logic.GetPiece(static_cast<Square>(i));
            if (c_p.first != Color::NoneColor && c_p.second != Piece::NonePiece)
                row += UI::Image::Get(c_p.first, c_p.second) + " | ";
            else row += "  | ";
        }
        newBoard = line + row + "\n" + newBoard;
    }
    newBoard += line + files;
    std::cout << newBoard;
}