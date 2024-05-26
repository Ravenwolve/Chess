// // For testing
// void PrintBitboard(Chess::Core::Bitboard bb) {
//     auto bitset = std::bitset<64>(bb);
//     int start = 56, end = 64;
//     for (int i = start; i < end && end != 0; ) {
//         std::cout << (bitset[i]);
//         ++i;
//         if (i == end) {
//             end = start;
//             start -= 8;
//             i = start;
//             std::cout << std::endl;
//         }
//     }
// }
#include <CLI/Console.hpp>
#include <iostream>


int main() {
    Chess::UI::CLI obj;
    obj.Open();
}