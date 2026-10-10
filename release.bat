@echo off
setlocal enabledelayedexpansion

echo Compilando SDL3 en modo RELEASE...

REM Borrar objetos antiguos
del *.o 2>nul

REM Variable para acumular objetos
set OBJ_FILES=

REM Compilar cada .cpp en src con optimización
for %%f in (src\*.cpp) do (
    echo Compilando %%f
    g++ -c "%%f" -std=c++17 -O3 -DNDEBUG -Wall -Wextra -I include -I lib\libsdl3\64\include
    set OBJ_FILES=!OBJ_FILES! %%~nf.o
)

REM Linkear todos los objetos con optimización RELEASE
echo Linkeando...
g++ !OBJ_FILES! -o bin\64\release\main.exe -L lib\libsdl3\64\lib -lmingw32 -lSDL3 -lSDL3_image

if errorlevel 1 (
    echo ERROR: compilacion RELEASE fallida
    del *.o 2>nul
    exit /b 1
)

echo Compilacion RELEASE exitosa
del *.o 2>nul
start "" bin\64\release\main.exe
exit /b 0