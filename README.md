## Introduction
This repository contains a static C++ library for encoding and decoding ASAM Control Module Protocol messages. ASAM CMP is used to monitor in-vehicle bus communication and sensor data. 

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
You can enable tests with the `ASAM_CMP_ENABLE_TESTS` cmake option. You can use the `ASAM_CMP_BUILD_EXAMPLE` cmake option to build a usage example.
To compile the library with tests in Windows using Visual Studio 2022 use command line:
```
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DASAM_CMP_ENABLE_TESTS=ON
cmake --build build
```
To compile the library with tests in Linux use command line:
```
cmake -S . -B build -DASAM_CMP_ENABLE_TESTS=ON
cmake --build build
```

## Usage
A usage example can be found in the `example` directory. Use the `ASAM_CMP_BUILD_EXAMPLE` cmake option to build a usage example.
