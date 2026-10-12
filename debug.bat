@echo off
setlocal enabledelayedexpansion

echo Compiling SDL3 in DEBUG mode...

taskkill /f /im main.exe 2>nul

REM Delete old object files
del *.o 2>nul

REM Variable to accumulate object files
set OBJ_FILES=

REM Compile each .cpp in src
for %%f in (src\*.cpp) do (
    echo Compiling %%f
    g++ -c "%%f" -std=c++17 -g -Wall -Wextra -I include -I lib\libsdl3\64\include
    set OBJ_FILES=!OBJ_FILES! %%~nf.o
)

REM Link all object files with DEBUG flags
echo Linking...
g++ !OBJ_FILES! -o bin\64\debug\main.exe -L lib\libsdl3\64\lib -lmingw32 -lSDL3 -lSDL3_image

if errorlevel 1 (
    echo ERROR: DEBUG compilation failed
	del *.o 2>nul
    exit /b 1
)

echo DEBUG compilation successful
del *.o 2>nul
start "" bin\64\debug\main.exe
exit /b 0