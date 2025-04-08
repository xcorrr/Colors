//Colors.exe by xcorr. See my other projects here: https://github.com/xcorrr
#include <windows.h>
#include <iostream>
#include <map>

std::map<std::string, WORD> colorMap = {
    {"black",         0},
    {"blue",          1},
    {"green",         2},
    {"aqua",          3},
    {"red",           4},
    {"purple",        5},
    {"yellow",        6},
    {"white",         7},
    {"gray",          8},
    {"lightblue",     9},
    {"lightgreen",   10},
    {"lightaqua",    11},
    {"lightred",     12},
    {"lightpurple",  13},
    {"lightyellow",  14},
    {"brightwhite",  15}
};

void printColored(const std::string& color, const std::string& message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Save the current attributes
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalColor = csbi.wAttributes;

    if (colorMap.find(color) != colorMap.end()) {
        SetConsoleTextAttribute(hConsole, colorMap[color]);
    }

    std::cout << message;

    // Reset color
    SetConsoleTextAttribute(hConsole, originalColor);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: colors.exe <color> <message>\n";
        return 1;
    }
    if (std::string(argv[1]) == "/?") {
        std::cout << "Colors! - A tool developed by xcorr.\n";
        std::cout << "Github: https://github.com/xcorrr\n";
        std::cout << "Usage: colors.exe <color> <message>\n";
        std::cout << "Available colors:\n";
        for (const auto& pair : colorMap) {
            std::cout << "  " << pair.first << "\n";
        }
        std::cout << "Example: colors.exe red Hello, User!\n";
        return 0;
    }
    if (std::string(argv[1]) == "-ver" || std::string(argv[1]) == "--version") {
        std::cout << "Colors! - A tool developed by xcorr.\n";
        std::cout << "Version: 1.0.0\n";
        return 0;
    }
    std::string color = argv[1];
    std::string message;

    // Join the rest of the args as the message
    for (int i = 2; i < argc; ++i) {
        message += argv[i];
        if (i < argc - 1) message += " ";
    }

    printColored(color, message);
    return 0;
}