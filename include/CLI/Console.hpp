#include <Logic/Logic.hpp>

namespace Chess::UI {
    class CLI {
        Logic logic;
        const static std::string line;
        const static std::string files;
        std::string strBoard;
    public:
        void Refresh();
    };
}