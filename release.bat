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
    g++ -c "%%f" -std=c++14 -O3 -DNDEBUG -I include -I lib\libsdl3\64\include
    set OBJ_FILES=!OBJ_FILES! %%~nf.o
)

REM Linkear todos los objetos con optimización RELEASE
echo Linkeando...
g++ !OBJ_FILES! -o bin\release\64\main.exe -L lib\libsdl3\64\lib -lmingw32 -lSDL3 -lSDL3_image

if errorlevel 1 (
    echo ERROR: compilacion RELEASE fallida
    exit /b 1
)

echo Compilacion RELEASE exitosa
start "" bin\64\debug\main.exe
exit /b 0