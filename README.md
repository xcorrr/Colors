# Colors
**colors.exe - Command line color output for batch scripts**

# How to Use:
Use this program inside your Batch scripts to print colored messages to the terminal.

Syntax:
    ***colors <color> <message>***

Examples:
    ***colors red "Err: Something went wrong!"***
    ***colors green "Success: Task completed!"***
    ***colors yellow "Warning: Check your input."***

# Supported Colors:
  **black      ,darkblue     ,darkgreen    ,darkaqua**
  **,darkred    ,darkpurple   ,darkyellow   ,gray**
  **,darkgray   ,blue         ,green        ,aqua**
  **,red        ,purple       ,yellow       ,white.**

# How It Works:
This tool changes the console text color, prints your message, 
then resets the color back to default.

# How to Compile (optional):
If you want to compile from source:
    ***g++ color_src.cpp -o colors.exe**

# Other Commands:
***colors -help*** - shows help panel in console
***colors -ver*** - prints version (oh, and --version too.)

Author: **xcorr**
Website: **https://xcorr.neocities.org**
