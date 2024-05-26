#include <CLI/Console.hpp>
#include <CLI/Image.hpp>
#include <iostream>

using enum Chess::Types::Square;

const std::string Chess::UI::CLI::line = "  +---+---+---+---+---+---+---+---+\n";
const std::string Chess::UI::CLI::files = "    A   B   C   D   E   F   G   H\n";

const static std::unordered_map<std::string, Chess::Types::Square> squares {
    {"A1", A1}, {"A2", A2}, {"A3", A3}, {"A4", A4}, {"A5", A5}, {"A6", A6}, {"A7", A7}, {"A8", A8},
    {"B1", B1}, {"B2", B2}, {"B3", B3}, {"B4", B4}, {"B5", B5}, {"B6", B6}, {"B7", B7}, {"B8", B8},
    {"C1", C1}, {"C2", C2}, {"C3", C3}, {"C4", C4}, {"C5", C5}, {"C6", C6}, {"C7", C7}, {"C8", C8},
    {"D1", D1}, {"D2", D2}, {"D3", D3}, {"D4", D4}, {"D5", D5}, {"D6", D6}, {"D7", D7}, {"D8", D8},
    {"E1", E1}, {"E2", E2}, {"E3", E3}, {"E4", E4}, {"E5", E5}, {"E6", E6}, {"E7", E7}, {"E8", E8},
    {"F1", F1}, {"F2", F2}, {"F3", F3}, {"F4", F4}, {"F5", F5}, {"F6", F6}, {"F7", F7}, {"F8", F8},
    {"G1", G1}, {"G2", G2}, {"G3", G3}, {"G4", G4}, {"G5", G5}, {"G6", G6}, {"G7", G7}, {"G8", G8},
    {"H1", H1}, {"H2", H2}, {"H3", H3}, {"H4", H4}, {"H5", H5}, {"H6", H6}, {"H7", H7}, {"H8", H8}
};

void Chess::UI::CLI::Refresh() {
    std::flush(std::cout);
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

void Chess::UI::CLI::Move(const std::string& from, const std::string& to) {
    try {
        logic.Move(squares.at(from), squares.at(to));
    }
    catch (...) {
        throw std::runtime_error("Incorrect square");
    }
}

void Chess::UI::CLI::Open() {
    std::string from, to;
    while (true) {
        Refresh();
        if (logic.IsCheckmate()) {
            std::cout << "It's checkmate! " << (logic.PlayerNow() == Types::Color::White ? "White": "Black") << " win!\n";
            return;
        }
        else if (logic.IsCheck())
            std::cout << "It's check!\n";
        else if (logic.IsStalemate()) {
            std::cout << "It's stalemate!\n";
            return;
        }
        std::cout << "Enter FROM square TO square for move:\n";
        std::cin >> from >> to;
        try {
            Move(from, to);
        }
        catch (...) {
            std::cout << "It's mistake! Try again.\n";
        }
    }
}