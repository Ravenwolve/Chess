#pragma once
#include <Logic/Logic.hpp>
#include <unordered_map>

namespace Chess::UI {
    class CLI {
        Logic logic;
        const static std::string line;
        const static std::string files;
        std::string strBoard;
        
    public:
        void Refresh();
        void Move(const std::string& from, const std::string& to);
        void Open();
    };
}