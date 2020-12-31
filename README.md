# Firedistra
A shitty 2D Game Engine using C++ and SDL2 that I'm building on my spare time.

![alt text](https://github.com/ByRussX/Firedistra/blob/master/extras/big-logo.png)


# Tools used
`GCC` 8.3.0 (I used `MinGW` http://www.mingw.org/)

`SDL2` (Included in <a href = https://github.com/ByRussX/Firedistra/tree/master/dependencies>dependencies</a>, but here is the link to the download site https://www.libsdl.org/download-2.0.php)

# Commands to build
Build Debug:

```
g++ -c src/*.cpp -std=c++17 -g -Wall -I include/ -I dependencies/include/ && g++ *.o -o bin/debug/main -L dependencies/lib/ -lmingw32 -lSDL2main -lSDL2 -lSDL2_image && start bin/debug/main
```

Build Release:

```
g++ -c src/*.cpp -std=c++17 -O3 -Wall -I include/ -I dependencies/include/ && g++ *.o -o bin/release/main -s -L dependencies/lib/ -lmingw32 -lSDL2main -lSDL2 -lSDL2_image && start bin/release/main
```

# OS Support
I have been doing this project on Windows 7, so it will work on Windows 10. I haven't made any OS dependant changes, so it should also work on Linux.
Don't ask me about MacOS. I hate Apple, and I have no idea about if it would work.

The only thing is that for both Linux and MacOS you will have to download and install SDL2 for the respective OS, and change the dependencies' DLLs and A files(I think you will also have to change the header files).

Good luck.

# Contact
If you need help with something, you can contact me on Discord (https://discord.com/9FFhUvx)
Don't get to high hopes tho. I am also a noob, so ask on StackOverflow (suggestion)

Join my Discord anyway (xd)
