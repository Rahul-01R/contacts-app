@echo off

g++ -std=c++17 -Wall -Wextra -g -Ilibs ^
src/main.cpp ^
src/ContactBook.cpp ^
src/Normalize.cpp ^
src/Storage.cpp ^
src/VCard.cpp ^
-o server.exe -lws2_32

if %errorlevel% neq 0 (
    echo.
    echo BUILD FAILED!
    pause
    exit /b 1
)

echo.
echo BUILD SUCCESSFUL!
echo.

pause
