
## Steps to use LovyanGFX with Visual Studio Code + PlatformIO + SDL2

First, install Visual Studio Code and set up PlatformIO so it is ready to use.  

 ---

### Set up PlatformIO to be able to build with `platform = native`.

Follow the instructions by reading the `Installation` section at the URL below.  
https://docs.platformio.org/en/latest/platforms/native.html#installation

#### For Linux

Install `build-essential` using `apt`.
```
sudo apt update
sudo apt install build-essential
```

#### For macOS
Install `xcode-select` from the terminal.
```
xcode-select --install
```

#### For Windows
Obtain and install `MSYS2` from https://www.msys2.org/.
Then, open Windows `System Properties` -> `Environment Variables`, and add the following 3 paths to `PATH`.
```
C:\msys64\mingw32\bin
C:\msys64\ucrt64\bin
C:\msys64\usr\bin
```


 ---

### Set up SDL2 for use with PlatformIO.

Follow the instructions by reading the `Install SDL2` section at the URL below.  
https://docs.lvgl.io/latest/en/html/get-started/pc-simulator.html#install-sdl-2


#### For Linux

Install libsdl2 using apt-get.

```
sudo apt-get install libsdl2 libsdl2-dev
```

#### For macOS

Install sdl2 using Homebrew.
```
 brew install sdl2
```

#### For Windows

Since you should have already installed msys2 in the steps to enable `platform = native` builds,
go to the SDL repository on GitHub and obtain the SDL2-devel-x.xx.x-mingw release package.  
https://github.com/libsdl-org/SDL/releases

At the time of writing, the filename is `SDL2-devel-2.28.1-mingw.zip`  
Extract this archive, open the `x86_64-w64-mingw32` folder inside the extracted directory, and verify that it contains the following 4 folders.
 - share
 - bin
 - include
 - lib

Open C:\msys64\mingw32\, verify that folders with the same names as the above 4 folders exist, then copy the contents of the above folders into C:\msys64\mingw32\. (Overwrite copy)

 ---




