# AsyncServerParent
A multithreaded C++ framework for TCP servers built with Boost Asio.  This repository has been maintained to handle designs and issues I've come across in the many projects I've made using these dependencies.  By overriding a few methods, AsyncServerParent can handle WSS, HTTP, plain TCP, or even UDP.

## Building  
Requirements: Boost (1.68.0)  

#### Requirement Installation
1. Installing Boost
    1. Download from https://www.boost.org/  
    2. In boost root directory, run ```./bootstrap```  
    3. For default configuration compatibility run: ```./b2 --stagedir=./stage/VS2017/x64 address-model=64 --toolset=msvc-14.1 --with-thread variant=release link=static threading=multi runtime-link=static```  
    4. Add the boost root directory to CMAKE_INCLUDE_PATH and the directory of your boost library files (should be in stage/lib) to CMAKE_LIBRARY_PATH.

#### Build
```
mkdir build
cd build
cmake .. -G "Visual Studio 15 2017" -A x64 -T host=x64        #For Ubuntu don't include -G option
```
Run make or build the solution  

## Examples
See [AsyncCppServer](https://github.com/Jester565/AsyncCppServer).
