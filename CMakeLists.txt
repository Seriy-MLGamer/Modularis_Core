#(C) 2023-2024 Серый MLGamer. All freedoms preserved.
#Дзен: <https://dzen.ru/seriy_mlgamer>
#SoundCloud: <https://soundcloud.com/seriy_mlgamer>
#YouTube: <https://www.youtube.com/@Seriy_MLGamer>
#GitHub: <https://github.com/Seriy-MLGamer>
#E-mail: <Seriy-MLGamer@yandex.ru>
#
#This file is part of Modularis Core.
#Modularis Core is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#Modularis Core is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#You should have received a copy of the GNU General Public License along with Modularis Core. If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.25)

include(./configuration.cmake)

if(BUILD_TARGET STREQUAL host)
	if(COMPILER STREQUAL GCC)
		set(MSVC NO)
		set(CMAKE_C_COMPILER gcc)
		set(CMAKE_CXX_COMPILER g++)
	endif()
else()
	if(BUILD_TARGET STREQUAL x86_64-linux-gnu)
		set(CMAKE_SYSTEM_NAME Linux)
		set(CMAKE_SYSTEM_PROCESSOR amd64)
	elseif(BUILD_TARGET STREQUAL i386-linux-gnu OR BUILD_TARGET STREQUAL i686-linux-gnu)
		set(CMAKE_SYSTEM_NAME Linux)
		set(CMAKE_SYSTEM_PROCESSOR i386)
	elseif(BUILD_TARGET STREQUAL aarch64-linux-gnu)
		set(CMAKE_SYSTEM_NAME Linux)
		set(CMAKE_SYSTEM_PROCESSOR arm64)
	elseif(BUILD_TARGET STREQUAL arm-linux-gnu)
		set(CMAKE_SYSTEM_NAME Linux)
		set(CMAKE_SYSTEM_PROCESSOR arm)
	elseif(BUILD_TARGET STREQUAL arm-linux-gnueabi)
		set(CMAKE_SYSTEM_NAME Linux)
		set(CMAKE_SYSTEM_PROCESSOR armel)
	elseif(BUILD_TARGET STREQUAL arm-linux-gnueabihf)
		set(CMAKE_SYSTEM_NAME Linux)
		set(CMAKE_SYSTEM_PROCESSOR armhf)
	elseif(BUILD_TARGET STREQUAL x86_64-w64-mingw32)
		set(CMAKE_SYSTEM_NAME Windows)
		set(CMAKE_SYSTEM_PROCESSOR amd64)
	elseif(BUILD_TARGET STREQUAL i686-w64-mingw32)
		set(CMAKE_SYSTEM_NAME Windows)
		set(CMAKE_SYSTEM_PROCESSOR i686)
	endif()
	if(BUILD_TARGET STREQUAL i686-linux-gnu)
		set(MULTIARCH i386-linux-gnu)
		if(COMPILER STREQUAL GCC)
			set(MSVC NO)
			set(CMAKE_C_COMPILER i686-linux-gnu-gcc)
			set(CMAKE_CXX_COMPILER i686-linux-gnu-g++)
		endif()
	else()
		set(MULTIARCH ${BUILD_TARGET})
		if(COMPILER STREQUAL GCC)
			set(MSVC NO)
			set(CMAKE_C_COMPILER ${MULTIARCH}-gcc)
			set(CMAKE_CXX_COMPILER ${MULTIARCH}-g++)
		endif()
	endif()
endif()

project(Modularis_Core
	HOMEPAGE_URL https://github.com/Seriy-MLGamer/Modularis_Core
	LANGUAGES C CXX
)

if(BUILD_TARGET STREQUAL host)
	if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL x86_64 OR CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL AMD64)
		set(CMAKE_SYSTEM_PROCESSOR amd64)
		if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
			set(MULTIARCH x86_64-linux-gnu)
		elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
			set(MULTIARCH x86_64-w64-mingw32)
		endif()
	elseif(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL i386)
		set(CMAKE_SYSTEM_PROCESSOR i386)
		if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
			set(MULTIARCH i386-linux-gnu)
		endif()
	elseif(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL i686)
		if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
			set(CMAKE_SYSTEM_PROCESSOR i386)
			set(MULTIARCH i386-linux-gnu)
		elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
			set(CMAKE_SYSTEM_PROCESSOR i686)
			set(MULTIARCH i686-w64-mingw32)
		endif()
	elseif(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL aarch64)
		set(CMAKE_SYSTEM_PROCESSOR arm64)
		if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
			set(MULTIARCH aarch64-linux-gnu)
		endif()
	elseif(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL arm)
		set(CMAKE_SYSTEM_PROCESSOR arm)
		if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
			set(MULTIARCH arm-linux-gnu)
		endif()
	elseif(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL armeabi)
		set(CMAKE_SYSTEM_PROCESSOR armel)
		if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
			set(MULTIARCH arm-linux-gnueabi)
		endif()
	elseif(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL armeabihf)
		set(CMAKE_SYSTEM_PROCESSOR armhf)
		if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
			set(MULTIARCH arm-linux-gnueabihf)
		endif()
	else()
		set(CMAKE_SYSTEM_PROCESSOR amd64)
		if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
			set(MULTIARCH x86_64-linux-gnu)
		elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
			set(MULTIARCH x86_64-w64-mingw32)
		endif()
	endif()
endif()
if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
	set(CMAKE_INSTALL_PREFIX "${GNU_INSTALLATION_FOLDER}")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
	set(CMAKE_INSTALL_PREFIX "${WINDOWS_INSTALLATION_FOLDER}")
endif()
set(CMAKE_INSTALL_DOCDIR share/doc)

set(CPACK_PACKAGE_NAME libmodularis)
set(CPACK_PACKAGE_VERSION 0.0.0pre-alpha-raw)
set(CPACK_PACKAGE_VENDOR "Серый MLGamer <Seriy-MLGamer@yandex.ru>")
set(CPACK_RESOURCE_FILE_LICENSE ../COPYING.md)
set(DESCRIPTION_SUMMARY "Modularis Core modular sound synthesis framework")
set(DESCRIPTION "This is a free (as in freedom) modular synthesis framework for creation of free digital audio workstations (DAW) and other free programs. Modularis free modular DAW is based on it. Framework is fully crossplatform between mobile and desktop devices, supports various programming languages.

Framework essence
  * Freedom
Aim of this framework development is to make music art accessible for users of very different devices. Framework architecture is designed with flexibility and extensibility in mind to make possible developing wide range of programs based on this framework. Free license allows users to have benefits from natural information properties: permission to use the framework for any purpose, study and change its work, share the framework.
  * Modular synthesis
Synthesizers, effects and control tools are modules that can be connected to other modules at input and output ports. It is more efficient and flexible architecture for music writing than multitrack architecture. Modular synthesis architecture has more potential of performance, multithreading and memory economy.

Features
  * Use in your programs!
Modularis Core framework can be used as player of music compositions made in programs based on this framework. You can interact with them as you programmed it.
  * Make your DAW!
Modularis Core framework architecture makes it possible to create very different music editors, be it sequencer DAW, modular synthesizer, tracker or even drum machine!
  * Program music!
It is possible to make music using Modularis Core framework without any DAW. Choose the programming language supported by the framework, create an instance of Modularis class, instances of modules, connect them with each other, configure them and make sound!
  * Connect plugins! (not implemented yet)
Modularis Core framework supports VST3 and LV2 plugins. It also has its own flexible and extensible plugin system.
  * Save your compositions to files! (also not implemented)
Modularis Core framework implements extensible Modularis project file format based on JSON for possibility of manual editing (just in case). A file contains information about settings and connections of modules. It also can contain information for the DAW the project was created in (or something else). You can create either light file with external dependencies on plugins, samples or something else or heavy, but portable file with embedded dependencies.")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${MULTIARCH})
set(CPACK_COMPONENT_LIBMODULARIS0_DISPLAY_NAME "Runtime library")
set(CPACK_COMPONENT_LIBMODULARIS0_DESCRIPTION "${DESCRIPTION_SUMMARY}
${DESCRIPTION}

This package provides runtime shared library of the framework.")
set(CPACK_COMPONENT_LIBMODULARIS-DEV_DISPLAY_NAME "Development files")
set(CPACK_COMPONENT_LIBMODULARIS-DEV_DEPENDS libmodularis0)
set(CPACK_COMPONENT_LIBMODULARIS-DEV_DESCRIPTION "${DESCRIPTION_SUMMARY} - development files
${DESCRIPTION}

This package provides files for software development on the framework.")
set(CPACK_COMPONENT_LIBMODULARIS-CPP-DEV_DISPLAY_NAME "Development files")
set(CPACK_COMPONENT_LIBMODULARIS-CPP-DEV_DEPENDS libmodularis0 libmodularis-dev)
set(CPACK_COMPONENT_LIBMODULARIS-CPP-DEV_DESCRIPTION "Modularis Core C++ - development files for C++
${DESCRIPTION}

This package provides a C++ language binding to Modularis Core library written in C for software development in C++ language.")
if(PACKAGE_TYPE STREQUAL host)
	if(CMAKE_SYSTEM_NAME STREQUAL Linux)
		set(PACKAGE_TYPE tar.gz)
	elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
		set(PACKAGE_TYPE zip)
	endif()
endif()
if(PACKAGE_TYPE STREQUAL tar.gz)
	set(CPACK_GENERATOR TGZ)
	add_custom_target(changelog.gz ALL
		COMMAND gzip -kf9 ../src/packaging/deb/changelog
		BYPRODUCTS ../src/packaging/deb/changelog.gz
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog.gz
		COMPONENT libmodularis0
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis0
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog.gz
		COMPONENT libmodularis-dev
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis-dev
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog.gz
		COMPONENT libmodularis-cpp-dev
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis-cpp-dev
	)
elseif(PACKAGE_TYPE STREQUAL zip)
	set(CPACK_GENERATOR ZIP)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog
		COMPONENT libmodularis0
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis0
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog
		COMPONENT libmodularis-dev
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis-dev
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog
		COMPONENT libmodularis-cpp-dev
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis-cpp-dev
	)
elseif(PACKAGE_TYPE STREQUAL deb)
	set(CPACK_GENERATOR DEB)
	set(CPACK_DEBIAN_PACKAGE_SOURCE libmodularis)
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Серый MLGamer <Seriy-MLGamer@yandex.ru>")
	set(CPACK_DEBIAN_PACKAGE_PRIORITY optional)
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
	set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
	set(CPACK_DEB_COMPONENT_INSTALL ON)
	set(CPACK_DEBIAN_LIBMODULARIS0_PACKAGE_NAME libmodularis0)
	set(CPACK_DEBIAN_LIBMODULARIS0_PACKAGE_SECTION libs)
	set(CPACK_DEBIAN_LIBMODULARIS0_PACKAGE_DEPENDS libc6)
	set(CPACK_DEBIAN_LIBMODULARIS0_PACKAGE_CONTROL_EXTRA
		../src/packaging/deb/shlibs
		../src/packaging/deb/triggers
	)
	set(CPACK_DEBIAN_LIBMODULARIS-DEV_PACKAGE_NAME libmodularis-dev)
	set(CPACK_DEBIAN_LIBMODULARIS-DEV_PACKAGE_SECTION libdevel)
	set(CPACK_DEBIAN_LIBMODULARIS-DEV_PACKAGE_DEPENDS libmodularis0)
	set(CPACK_DEBIAN_LIBMODULARIS-CPP-DEV_PACKAGE_NAME libmodularis-cpp-dev)
	set(CPACK_DEBIAN_LIBMODULARIS-CPP-DEV_PACKAGE_SECTION libdevel)
	set(CPACK_DEBIAN_LIBMODULARIS-CPP-DEV_PACKAGE_DEPENDS "libmodularis0, libmodularis-dev")
	add_custom_target(changelog.Debian.gz ALL
		COMMAND gzip -kf9 ../src/packaging/deb/changelog
		COMMAND mv ../src/packaging/deb/changelog.gz ../src/packaging/deb/changelog.Debian.gz
		BYPRODUCTS ../src/packaging/deb/changelog.Debian.gz
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog.Debian.gz
		COMPONENT libmodularis0
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis0
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog.Debian.gz
		COMPONENT libmodularis-dev
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis-dev
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog.Debian.gz
		COMPONENT libmodularis-cpp-dev
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis-cpp-dev
	)
	set(CMAKE_INSTALL_LIBDIR lib/${MULTIARCH})
elseif(PACKAGE_TYPE STREQUAL NSIS)
	set(CPACK_GENERATOR NSIS)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog
		COMPONENT libmodularis0
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis0
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog
		COMPONENT libmodularis-dev
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis-dev
	)
	install(FILES
			src/packaging/deb/copyright
			src/packaging/deb/changelog
		COMPONENT libmodularis-cpp-dev
		DESTINATION ${CMAKE_INSTALL_DOCDIR}/libmodularis-cpp-dev
	)
endif()

include(GNUInstallDirs)

if(NOT MSVC)
	add_compile_options(-O -s -fms-extensions)
	add_link_options(-O -s)
endif()
include_directories(include)
add_library(modularis SHARED
	src/framework/libmodularis0/modules/players/Sequencer.c
	src/framework/libmodularis0/modules/synthesizers/Oscillator.c
	src/framework/libmodularis0/modules/system/Module.c
	src/framework/libmodularis0/modules/system/Output.c
	src/framework/libmodularis0/ports/controllers/ADSR.c
	src/framework/libmodularis0/ports/controllers/Integer_controller.c
	src/framework/libmodularis0/ports/controllers/Real_controller.c
	src/framework/libmodularis0/ports/system/Port.c
	src/framework/libmodularis0/ports/system/Ports_folder.c
	src/framework/libmodularis0/ports/Note.c
	src/framework/libmodularis0/ports/Sound.c
	src/framework/libmodularis0/Modularis.c
)
set_property(TARGET modularis PROPERTY SOVERSION 0)
add_library(modularis-cpp STATIC
	src/framework/libmodularis-cpp-dev/modules/players/Sequencer.cpp
	src/framework/libmodularis-cpp-dev/modules/synthesizers/Oscillator.cpp
	src/framework/libmodularis-cpp-dev/modules/system/Module.cpp
	src/framework/libmodularis-cpp-dev/modules/system/Output.cpp
	src/framework/libmodularis-cpp-dev/ports/controllers/ADSR.cpp
	src/framework/libmodularis-cpp-dev/ports/controllers/Integer_controller.cpp
	src/framework/libmodularis-cpp-dev/ports/controllers/Real_controller.cpp
	src/framework/libmodularis-cpp-dev/ports/system/Port.cpp
	src/framework/libmodularis-cpp-dev/ports/system/Ports_folder.cpp
	src/framework/libmodularis-cpp-dev/ports/Note.cpp
	src/framework/libmodularis-cpp-dev/ports/Sound.cpp
	src/framework/libmodularis-cpp-dev/Modularis.cpp
)
install(TARGETS modularis
	RUNTIME
		COMPONENT libmodularis0
	LIBRARY
		COMPONENT libmodularis0
		NAMELINK_COMPONENT libmodularis-dev
	ARCHIVE
		COMPONENT libmodularis-dev
)
install(DIRECTORY "include/Modularis_Core"
	COMPONENT libmodularis-dev
	TYPE INCLUDE
)
install(TARGETS modularis-cpp
	COMPONENT libmodularis-cpp-dev
)
install(DIRECTORY "include/Modularis_Core_C++"
	COMPONENT libmodularis-cpp-dev
	TYPE INCLUDE
)
if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
	add_custom_target(uninstall
		COMMAND -rm "${CMAKE_INSTALL_FULL_LIBDIR}/libmodularis-cpp.a"
		COMMAND -rm -r "${CMAKE_INSTALL_FULL_INCLUDEDIR}/Modularis_Core_C++"
		COMMAND -rm "${CMAKE_INSTALL_FULL_LIBDIR}/libmodularis.so"
		COMMAND -rm -r "${CMAKE_INSTALL_FULL_INCLUDEDIR}/Modularis_Core"
		COMMAND -rm -r "${CMAKE_INSTALL_FULL_DOCDIR}/libmodularis0"
		COMMAND -rm "${CMAKE_INSTALL_FULL_LIBDIR}/libmodularis.so.0"
		COMMAND -rm -r "${CMAKE_INSTALL_FULL_DOCDIR}/libmodularis-dev"
	)
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
	set(CMAKE_SHARED_LIBRARY_SUFFIX -0.dll)
	add_custom_target(uninstall
		COMMAND -rmdir /S /Q "${CMAKE_INSTALL_PREFIX}"
	)
endif()

include(CPack)