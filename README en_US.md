<!--
(C) 2022-2024 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is free documentation: you can redistribute it and/or modify it under the terms of the Creative Commons Attribution-ShareAlike 4.0 International license: <https://creativecommons.org/licenses/by-sa/4.0/>.
This file is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
-->

# Modularis Core

![Modularis Core full logo](share/logo%20en_US.svg)

This is a free (as in freedom) modular synthesis framework for creation of free digital audio workstations (DAW) and other free programs. [Modularis](https://github.com/Seriy-MLGamer/Modularis) free modular DAW is based on it. Framework is ***fully* crossplatform** between mobile and desktop devices, supports various programming languages.

## Framework essence

### Freedom

Aim of this framework development is to make music art accessible for users of very different devices. Framework architecture is designed with flexibility and extensibility in mind to make possible developing wide range of programs based on this framework. Free license allows users to have benefits from natural information properties: permission to use the framework for any purpose, study and change its work, share the framework.

### Modular synthesis

Synthesizers, effects and control tools are modules that can be connected to other modules at input and output ports. It is more efficient and flexible architecture for music writing than multitrack architecture. Modular synthesis architecture has more potential of performance, multithreading and memory economy.

## Features

### Use in your programs!

*Modularis Core* framework can be used as **player** of music compositions made in programs based on this framework. You can **interact** with them as you programmed it.

### Make your DAW!

*Modularis Core* framework architecture makes it possible to create ***very different* music editors**, be it sequencer DAW, modular synthesizer, tracker or even drum machine!

### Program music!

It is possible to make music using *Modularis Core* framework **without any DAW**. Choose the programming language supported by the framework, create an instance of `Modularis` class, instances of modules, connect them with each other, configure them and **make sound**!

### Connect plugins! (not implemented yet)

*Modularis Core* framework supports **VST3** and **LV2** plugins. It also has its own flexible and extensible plugin system.

### Save your compositions to files! (also not implemented)

*Modularis Core* framework implements **extensible Modularis project file format** based on JSON for possibility of manual editing (just in case). A file contains information about settings and connections of modules. It also can contain information for the DAW the project was created in (or something else). You can create either light file with external dependencies on plugins, samples or something else or heavy, but portable file with embedded dependencies.

# License

Modularis Core is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Modularis Core is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Modularis Core. If not, see <https://www.gnu.org/licenses/>.

![GNU General Public License version 3](https://www.gnu.org/graphics/gplv3-with-text-136x68.png)

***Only* free** (*as in freedom*) software under the terms of the GNU General Public License can be based on the *Modularis Core* framework.

# Everything is just continuing! The framework is expanding!

## Modularis Core 0.0.0pre-alpha-raw

The framework has been rewritten in C and now supports 2 programming languages: C and C++. C++ support is done using the language binding - Modularis Core C++.

The framework contents has not been changed yet.

## Framework contents

  * Modularis

It has feature of singlethreaded sound synthesis. It has "lazy update" mode - modules that are not connected, directly or indirectly, to the Output module are not updated. System sound frame format is 32-bit floating point number.

### Modules

#### Control tools

  * Sequencer

It has feature of polyphonic notes playing with fixed frequency and velocity.

#### Synthesizers

  * Oscillator

It has feature of polyphonic sound playing with fixed envelope. Sound can be of one of the 4 waveform types: sine `0`, triangle `1`, saw `2`, square `3`.

### Ports

  * Note

The port to convey notes.

  * Sound

The port to convey one channel of sound wave.

#### Controllers

  * Integer_controller
  * Real_controller

#### System

  * Ports_folder

The ports group that can be connected to other ports and groups or disconnected from them as one.

Now you know about the framework features at this moment. It is time to test them!

## The Guide for Building, Packaging and Testing

### Dependencies

Before starting the following procedures it is required to install these build dependencies:

  * CMake

#### GNU/Linux

  * GCC (recommended; Clang work wasn't tested);
  * Make (or similar program, if it works);

#### Windows

  * mingw32-w64 GCC (recommended; Visual Studio compiler work wasn't tested);
  * mingw32-w64 Make (or similar program, if it works);

### Configuring

Before starting the following procedures you can configure them in "configuration.cmake" file. Configuring tips are in the file.

Then generate the build file using these commands:

#### Bash

	$ mkdir out
	$ cd out
	$ cmake .. -G "Unix Makefiles"

#### CMD

	>mkdir out
	>cd out
	>cmake .. -G "MinGW Makefiles"

### Building

It is done by this command:

#### Bash

	$ make

#### CMD

	>mingw32-make

### Installation

This command will install the framework:

#### Bash

	$ make install

#### CMD

	>mingw32-make install

### Uninstallation

This command will uninstall the framework from the installation folder:

#### Bash

	$ make uninstall

#### CMD

	>mingw32-make uninstall

### Packaging

Packaging is done using CPack utility that is the part of CMake. In order to make package files have right rights in GNU/Linux it is recommended to do packaging as the superuser:

#### Bash

	$ sudo cpack

#### CMD

	>cpack

### Testing

After the procedures above you can test the framework by building and running test programs which use this framework. One of them is written in C, other - in C++. They contain my little track.

Firstly, install the framework. Then install SDL2 library. After that run this command in "test" folder, if you want to test C:

#### Bash

	$ ./test-c

#### CMD

	>test-c

Or run this command, if you want to test C++:

#### Bash

	$ ./test-cpp

#### CMD

	>test-cpp

These build scripts expect working with GCC compiler. But, I think, manual test compilation using other compiler with similar compilation arguments will not be a big deal.

If you test in Windows, make sure that static and dynamic SDL2 library files are in "test" folder or SDL2 library is added to PATH (\<SDL2 root folder>\\bin), CPATH (\<SDL2 root folder>\\include) and LIBRARY_PATH (\<SDL2 root folder>\\lib) environment variables.

# Enjoy using this framework!