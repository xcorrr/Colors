# Colors
**colors.exe - Command line color output for batch scripts**

# How to Use:
Use this program inside your Batch or C++ scripts to print colored messages without the struggle.
NOTE: this program only works in windows. Linux versions have not been made.

**Syntax:**
    ***colors <color> <message>***

**Examples:**
    ***colors red "Err: Something went wrong!",***
    ***colors green "Success: Task completed!",***
    ***colors yellow "Warning: Check your input."***

Supported Colors:
  **black      ,darkblue     ,darkgreen    ,darkaqua**
  **,darkred    ,darkpurple   ,darkyellow   ,gray**
  **,darkgray   ,blue         ,green        ,aqua**
  **,red        ,purple       ,yellow       ,white.**

How It Works:
This tool changes the console text color, prints your message, 
then resets the color back to default.

How to Compile (optional):
If you want to compile from source:
    ***g++ -O2 -static -o Colors.exe Colors_src.cpp -ladvapi32 -luser32***

Other Commands:
***colors --help*** - shows help panel in console and ***colors -ver** - prints version (oh, and --version too.)
***Colors -install*** - adds program to PATH.
NOTE: this uses the registry (advapi32) and SendMessageTimeoutA (user32). MSVC will auto-link those via the #pragma comment(lib, ...) lines I added. If you're compiling with MinGW/g++, use the command above.

Author: **xcorr,**
Website: **https://xcorr.neocities.org**
