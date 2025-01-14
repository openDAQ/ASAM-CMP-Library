## Introduction
This repository contains a static C++ library for encoding and decoding [ASAM Capture Module Protocol](https://www.asam.net/standards/detail/cmp/) messages. ASAM CMP is used to monitor in-vehicle bus communication and sensor data. 

## Features
- Encode and decode ASAM CMP Data and Status Messages.
- Support CAN, CAN FD, LIN, Analog and Ethernet payloads for Data Messages.
- Support Capture Module and Interface payloads for Status Messages.
- Has a Status helper class that stores information about devices and their interfaces that are on a network.

## Required tools before building
 - [CMake 3.0](https://cmake.org/) or higher
 - [Git](https://git-scm.com/)
 - Compiler:
   - (msvc) Visual Studio 2017 or higher with installed Workload for C++
   - gcc
   
## Build the project
Tests can be excluded from the build by setting the cmake option `ASAM_CMP_LIB_ENABLE_TESTS` to `OFF`.
The Usage example can be excluded from the build by setting the cmake option `ASAM_CMP_LIB_BUILD_EXAMPLE` to `OFF`.
To compile the library in Windows using Visual Studio 2022 use command line:
```
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build
```
To compile the library in Linux use command line:
```
cmake -S . -B build
cmake --build build
```

## Usage
A usage example can be found in the `example` directory.
