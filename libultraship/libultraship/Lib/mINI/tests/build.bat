@echo off
IF %1.==. GOTO ERR
g++ -Wall -Wextra -std=c++17 -I./lest -I./../src -o %1.exe %1.cpp
GOTO END
:ERR
echo Use: %0 [test name]
:END
