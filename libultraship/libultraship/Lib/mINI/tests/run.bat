@echo off
IF %1.==. GOTO ERR
%1.exe -p
GOTO END
:ERR
echo Use: %0 [test name]
:END