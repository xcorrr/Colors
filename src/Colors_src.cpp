//Colors.exe by xcorr. See my other projects here: https://github.com/xcorrr
#include <windows.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <cctype>
#include <cstring>

#ifdef _MSC_VER
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "user32.lib")
#endif

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

std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

void printColored(const std::string& color, const std::string& message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Save the current attributes
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalColor = csbi.wAttributes;

    std::string lowerColor = toLower(color);
    auto it = colorMap.find(lowerColor);
    if (it != colorMap.end()) {
        SetConsoleTextAttribute(hConsole, it->second);
    } else {
        std::cerr << "Warning: unknown color \"" << color << "\", using default.\n";
    }

    std::cout << message << std::endl;

    // Reset color
    SetConsoleTextAttribute(hConsole, originalColor);
}

// Reads the current user's PATH value from the registry.
// Returns true on success (even if the value doesn't exist yet, in which case out is left empty).
bool readUserPath(std::string& out) {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    DWORD type = 0;
    DWORD size = 0;
    LONG result = RegQueryValueExA(hKey, "Path", nullptr, &type, nullptr, &size);
    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
        RegCloseKey(hKey);
        return false;
    }

    if (result == ERROR_FILE_NOT_FOUND || size == 0) {
        out.clear();
        RegCloseKey(hKey);
        return true;
    }

    std::string buffer(size, '\0');
    result = RegQueryValueExA(hKey, "Path", nullptr, &type,
                               reinterpret_cast<LPBYTE>(&buffer[0]), &size);
    RegCloseKey(hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    // Strip trailing null terminator(s) that RegQueryValueExA may include.
    while (!buffer.empty() && buffer.back() == '\0') {
        buffer.pop_back();
    }
    out = buffer;
    return true;
}

// Writes a new PATH value to the registry as REG_EXPAND_SZ (matches how Windows stores it).
bool writeUserPath(const std::string& newPath) {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    LONG result = RegSetValueExA(hKey, "Path", 0, REG_EXPAND_SZ,
                                  reinterpret_cast<const BYTE*>(newPath.c_str()),
                                  static_cast<DWORD>(newPath.size() + 1));
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

// Checks (case-insensitively) whether dir is already one of the ; separated entries in path.
bool pathContainsDir(const std::string& path, const std::string& dir) {
    std::string lowerPath = toLower(path);
    std::string lowerDir = toLower(dir);
    // Trim a trailing backslash from dir for consistent comparison.
    if (!lowerDir.empty() && lowerDir.back() == '\\') {
        lowerDir.pop_back();
    }

    size_t start = 0;
    while (start <= lowerPath.size()) {
        size_t end = lowerPath.find(';', start);
        if (end == std::string::npos) end = lowerPath.size();
        std::string entry = lowerPath.substr(start, end - start);
        if (!entry.empty() && entry.back() == '\\') entry.pop_back();
        if (entry == lowerDir) return true;
        start = end + 1;
    }
    return false;
}

void addToPathInteractive() {
    char exePath[MAX_PATH];
    DWORD len = GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    if (len == 0 || len == MAX_PATH) {
        std::cerr << "Error: could not determine the program's own location.\n";
        return;
    }

    std::string fullPath(exePath);
    size_t lastSlash = fullPath.find_last_of('\\');
    if (lastSlash == std::string::npos) {
        std::cerr << "Error: could not determine the program's directory.\n";
        return;
    }
    std::string exeDir = fullPath.substr(0, lastSlash);

    std::cout << "Colors: NOTE, This will add the following folder to your user PATH:\n";
    std::cout << "  " << exeDir << "\n";
    std::cout << "so you can run \"colors\" from any directory. Continue? (y/n): ";

    std::string answer;
    std::getline(std::cin, answer);
    std::string lowerAnswer = toLower(answer);
    if (lowerAnswer != "y" && lowerAnswer != "yes") {
        std::cout << "Colors: Cancelled. PATH was not modified.\n";
        return;
    }

    std::string currentPath;
    if (!readUserPath(currentPath)) {
        std::cerr << "Colors: Error, could not read your current PATH from the registry.\n";
        return;
    }

    if (pathContainsDir(currentPath, exeDir)) {
        std::cout << "Colors: This folder is already in your PATH. Nothing to do.\n";
        return;
    }

    std::string newPath = currentPath;
    if (!newPath.empty() && newPath.back() != ';') {
        newPath += ';';
    }
    newPath += exeDir;

    if (!writeUserPath(newPath)) {
        std::cerr << "Error: could not write the updated PATH to the registry.\n";
        std::cerr << "(No admin rights are needed for this, but the write still failed.)\n";
        return;
    }

    // Notify other processes (like Explorer) that the environment changed.
    SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                         reinterpret_cast<LPARAM>("Environment"),
                         SMTO_ABORTIFHUNG, 5000, nullptr);

    std::cout << "Done! Added to PATH.\n";
    std::cout << "Open a new terminal window for the change to take effect.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: colors.exe <color> <message>\n";
        return 1;
    }
    if (std::string(argv[1]) == "/?" || std::string(argv[1]) == "-help" || std::string(argv[1]) == "--help") {
        std::cout << "Colors! - A tool developed by xcorr.\n";
        std::cout << "Github: https://github.com/xcorrr\n";
        std::cout << "Usage: colors.exe <color> <message>\n";
        std::cout << "Available colors:\n";
        for (const auto& pair : colorMap) {
            std::cout << "  " << pair.first << "\n";
        }
        std::cout << "Example: colors.exe red Hello, User!\n";
        std::cout << "Run colors.exe -install to add this program to your PATH.\n";
        return 0;
    }
    if (std::string(argv[1]) == "-ver" || std::string(argv[1]) == "-version" || std::string(argv[1]) == "--version") {
        std::cout << "Colors! - A tool developed by xcorr.\n";
        std::cout << "Version: 1.3, last updated 7/8/2026.\n";
        return 0;
    }
    if (std::string(argv[1]) == "-install" || std::string(argv[1]) == "--install" || std::string(argv[1]) == "-addtopath") {
        addToPathInteractive();
        return 0;
    }
    if (argc < 3) {
        std::cerr << "Usage: colors.exe <color> <message>\n";
        return 1;
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
