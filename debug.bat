@echo off
setlocal enabledelayedexpansion

echo Compilando SDL3 en modo DEBUG...

taskkill /f /im main.exe 2>nul

REM Borrar objetos antiguos
del *.o 2>nul

REM Variable para acumular objetos
set OBJ_FILES=

REM Compilar cada .cpp en src
for %%f in (src\*.cpp) do (
    echo Compilando %%f
    g++ -c "%%f" -std=c++17 -g -Wall -Wextra -I include -I lib\libsdl3\64\include
    set OBJ_FILES=!OBJ_FILES! %%~nf.o
)

REM Linkear todos los objetos con flags DEBUG
echo Linkeando...
g++ !OBJ_FILES! -o bin\64\debug\main.exe -L lib\libsdl3\64\lib -lmingw32 -lSDL3 -lSDL3_image

if errorlevel 1 (
    echo ERROR: compilacion DEBUG fallida
	del *.o 2>nul
    exit /b 1
)

echo Compilacion DEBUG exitosa
del *.o 2>nul
start "" bin\64\debug\main.exe
exit /b 0