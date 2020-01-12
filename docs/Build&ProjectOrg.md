# Build

This project Uses CMake, and requires at least CMake 3.14. 

## CMake

CMake is a meta build system for C/C++ which can generate projects of various other build systems. There are generators like:

* Borland Makefiles
* MSYS Makefiles
* MinGW Makefiles
* NMake Makefiles
* NMake Makefiles JOM
* Ninja
* Unix Makefiles
* Watcom WMake
* Visual Studio 6
* Visual Studio 7
* Visual Studio 7 .NET 2003
* Visual Studio 8 2005
* Visual Studio 9 2008
* Visual Studio 10 2010
* Visual Studio 11 2012
* Visual Studio 12 2013
* Xcode

On windows, CMake GUI or Visual Studio can be used with cmake. But it helps to know how to do it from command line:

Go to project directory. 
```
mkdir build
cd build
cmake ..
make
```

See [guide](https://cliutils.gitlab.io/modern-cmake/chapters/intro/running.html).


We will be using Visual Studio, which has CMake integration. In order to open a project in Visual Studio, do 
` File -> Open -> Folder`
and select the project foler. 

Visual Studio will now run CMake. More specifically, it will run the Configure and Generate steps. The Configure steps sets up some CMake
cache files and parses the CMakeLists.txt files. The Generate steps generates build files using some generator. **By default, Visual Studio
uses the Ninja generator, so that's what we're currently using**. You usually don't have to worry about how a Ninja project works. 

After generation is done, you can select `game.exe` from `Select Startup Item...`, and run. Visual Studio will now build everything (including
dependencies like GLFW). 

There are different ways of setting CMake up. The two described in Professional CMake, Chapter 28, are `Superbuild Structure` and `Non-Superbuild-Structure`. 

`Superbuild Structure` uses the `ExternalProject_Add` command to bring in other libraries. I tried this last semester, and as described in the book, 
it is very powerful but also can require you to hard code binary paths for different platforms. But, a properly implemented superbuild project would
probably actually be best. 

`Non-Superbuild-Structure` uses the `Fetch_Content`, and I decided that it might be worth switching to it. It runs the download during the configure step,
instead of during the build. It also treats the dependencies more like they're a part of the project, and builds everything with the same settings.


# Project Organization

The project structure used here is based on [this guide](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html).

## apps

This will contain the "client code" - the applications which use the game engine. The game executable lives here. 

## cmake_modules 

This is like node_modules from NPM. It contains CMake scripts that can bring in various dependencies. Miscellanious CMake scripts could 
probably go here as well, or a new cmake folder could be created. 

## docs

Documentation should go here.

## include

This is where all the header files go. They are split up by library, so you have `include/core`, `include/platform`.
The guide recommends keeping this folder clean, so that it could be copied to the system's include directories folder, 
or copied wherever else and used as an include folder for this library. 

## src

This is where .cpp files go. There's also a CMakeLists file here, which sets up the libraries that make up the engine. 

## test

Unit tests go here. 

