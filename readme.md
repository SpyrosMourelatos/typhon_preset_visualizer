# Preset Comparer and viewer #
##  Installation ##
This project has been tested on macOs BigSur with intel processor but it is cross platform as it doesn't use any native dependancy(actually it does but it is wrapped under qt).

### Dependancies ###
- [Qt open source ](https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5) (tested with 5.14.2 with clang binaries but it will work on MANY versions )
- cmake (has beewn tested with 3.5 to 3.19.1)
- [conan](https://docs.conan.io/en/latest/installation.html) (It has been tested with 1.32 and python 3.9.5 (Yes you need python also))
- A c++ compiler that supports c++17 (tested with Apple clang 12.0.0 )
- protoc/libproto (no need to install it manually see bellow)
### The actual build steps ###
On the source directory run :
```sh
conan install . -if build
```
Then source the virtual environment that has been created on the build folder so for *nix:
```sh
source build/activate.sh
```
I am not quite sure for windows but I guess `./build/activate.ps1`.

Then configure cmake to find a path for qt and protoc with the flags `CMAKE_PREFIX_PATH` and `CMAKE_PROJECT_INCLUDE`.
Qt at *nix is mostly on `/USERS/<username>/Qt/5.14.2/clang_64` with `<username>` being your username so run something like:
```
cmake -B build -DCMAKE_PROJECT_INCLUDE=build/conan_paths.cmake -DCMAKE_PREFIX_PATH=/USERS/spyros/Qt/5.14.2/clang_64/ 
```
then build the project :
```
cmake --build build 
```
and the executable will be on the build folder with the name `Preset_Comparer.(something_os_specific)`

### About ###
Hi there , this project was initially made only for personal consumption while I was making the gui for the typhon editor
so it is important for me to clarify that : 
- It is not an official dreadbox project
- It has some bugs 
- Support from my side will be more in the form of advices and code snippets rather than actual bugfixes and feature requests.

That being said here is a small presentation of the projects insights:
 Typhon useses google's protocol buffers for its presets  ,the exact format can be found on Resources/preset.proto 
 though a downside of this format  is that it is not very expressive for the real scale of the values.For example most of our values are stored as
 float though the real scale is floats between zero and one.Also I like beer(see license)