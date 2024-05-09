#include <iostream>
#include "Bitboard.hpp"
#include <bitset>

// For testing
void PrintBitboard(Chess::Bitboard bb) {
    auto bitset = std::bitset<64>(bb);
    int start = 56, end = 64;
    for (int i = start; i < end && end != 0; ) {
        std::cout << (bitset[i]);
        ++i;
        if (i == end) {
            end = start;
            start -= 8;
            i = start;
            std::cout << std::endl;
        }
    }
}

int main()
{
    // Chess::Bitboard a = 5;
    // std::cout << a.CountOf0()
    // std::cout << Chess::Bitboards::pieces[1][1].Count();
    //Chess::PieceBitboardContainer a("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    for (int i = 0; i < 64; ++i) {
        //PrintBitboard(Chess::Mask::queenMasks[i]);
        std::cout << std::endl;
    }

}