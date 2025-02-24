#!/usr/bin/python3 -B

#(C) 2023-2025 Серый MLGamer. All freedoms preserved.
#Дзен: <https://dzen.ru/seriy_mlgamer>
#SoundCloud: <https://soundcloud.com/seriy_mlgamer>
#YouTube: <https://www.youtube.com/@Seriy_MLGamer>
#GitVerse: <https://gitverse.ru/Seriy_MLGamer>
#E-mail: <Seriy-MLGamer@yandex.ru>
#
#This file is part of Modularis Core.
#Modularis Core is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#Modularis Core is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#You should have received a copy of the GNU General Public License along with Modularis Core. If not, see <https://www.gnu.org/licenses/>.

from makepy import *

from configuration import *

import gzip

class Modularis_Core(MakePy):
	def license(self):
		super().license()
		print()
		print("Build script license:")
		print("(C) 2023-2025 Seriy MLGamer. All freedoms preserved.")
		print("Dzen: <https://dzen.ru/seriy_mlgamer>")
		print("SoundCloud: <https://soundcloud.com/seriy_mlgamer>")
		print("YouTube: <https://www.youtube.com/@Seriy_MLGamer>")
		print("GitVerse: <https://gitverse.ru/Seriy_MLGamer>")
		print("E-mail: <Seriy-MLGamer@yandex.ru>")
		print()
		print("This file is part of Modularis Core.")
		print("Modularis Core is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.")
		print("Modularis Core is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.")
		print("You should have received a copy of the GNU General Public License along with Modularis Core. If not, see <https://www.gnu.org/licenses/>.")
	def main(self, defines):
		global BUILD_TARGET
		global PACKAGE_TYPE
		global GNU_INSTALLATION_FOLDER
		global WINDOWS_INSTALLATION_FOLDER
		if "BUILD_TARGET" in defines: BUILD_TARGET=defines["BUILD_TARGET"]
		if BUILD_TARGET=="host":
			if self.HOST_ARCHITECTURE=="x86_64" or self.HOST_ARCHITECTURE=="AMD64":
				if self.HOST_OPERATING_SYSTEM=="Linux": MULTIARCH="x86_64-linux-gnu"
				elif self.HOST_OPERATING_SYSTEM=="Windows": MULTIARCH="x86_64-w64-mingw32"
			elif self.HOST_ARCHITECTURE=="i386" or self.HOST_ARCHITECTURE=="i686" or self.HOST_ARCHITECTURE=="x86":
				if self.HOST_OPERATING_SYSTEM=="Linux": MULTIARCH="i386-linux-gnu"
				elif self.HOST_OPERATING_SYSTEM=="Windows": MULTIARCH="i686-w64-mingw32"
			elif self.HOST_ARCHITECTURE=="aarch64" or self.HOST_ARCHITECTURE=="ARM64":
				if self.HOST_OPERATING_SYSTEM=="Linux": MULTIARCH="aarch64-linux-gnu"
				elif self.HOST_OPERATING_SYSTEM=="Windows": MULTIARCH="aarch64-w64-mingw32"
			elif "armv7" in self.HOST_ARCHITECTURE: MULTIARCH="arm-linux-gnueabihf"
			elif "arm" in self.HOST_ARCHITECTURE: MULTIARCH="arm-linux-gnueabi"
			else:
				if self.HOST_OPERATING_SYSTEM=="Linux": MULTIARCH="x86_64-linux-gnu"
				elif self.HOST_OPERATING_SYSTEM=="Windows": MULTIARCH="x86_64-w64-mingw32"
		else:
			self.CC=BUILD_TARGET+"-gcc"
			self.CXX=BUILD_TARGET+"-g++"
			if self.HOST_OPERATING_SYSTEM=="Linux": self.DLLTOOL=BUILD_TARGET+"-dlltool"
			if BUILD_TARGET=="i686-linux-gnu": MULTIARCH="i386-linux-gnu"
			else: MULTIARCH=BUILD_TARGET
			if BUILD_TARGET=="x86_64-linux-gnu":
				self.TARGET_ARCHITECTURE="x86_64"
				self.TARGET_OPERATING_SYSTEM="Linux"
			elif BUILD_TARGET=="i386-linux-gnu":
				self.TARGET_ARCHITECTURE="i386"
				self.TARGET_OPERATING_SYSTEM="Linux"
			elif BUILD_TARGET=="i686-linux-gnu":
				self.TARGET_ARCHITECTURE="i686"
				self.TARGET_OPERATING_SYSTEM="Linux"
			elif BUILD_TARGET=="aarch64-linux-gnu":
				self.TARGET_ARCHITECTURE="aarch64"
				self.TARGET_OPERATING_SYSTEM="Linux"
			elif BUILD_TARGET=="arm-linux-gnueabihf":
				self.TARGET_ARCHITECTURE="armv7l"
				self.TARGET_OPERATING_SYSTEM="Linux"
			elif BUILD_TARGET=="arm-linux-gnueabi":
				self.TARGET_ARCHITECTURE="arm"
				self.TARGET_OPERATING_SYSTEM="Linux"
			elif BUILD_TARGET=="x86_64-w64-mingw32":
				self.TARGET_ARCHITECTURE="AMD64"
				self.TARGET_OPERATING_SYSTEM="Windows"
			elif BUILD_TARGET=="i686-w64-mingw32":
				self.TARGET_ARCHITECTURE="x86"
				self.TARGET_OPERATING_SYSTEM="Windows"
			elif BUILD_TARGET=="aarch64-w64-mingw32":
				self.TARGET_ARCHITECTURE="ARM64"
				self.TARGET_OPERATING_SYSTEM="Windows"
		if "PACKAGE_TYPE" in defines: PACKAGE_TYPE=defines["PACKAGE_TYPE"]
		if PACKAGE_TYPE=="host":
			if self.TARGET_OPERATING_SYSTEM=="Linux": PACKAGE_TYPE="tar.gz"
			elif self.TARGET_OPERATING_SYSTEM=="Windows": PACKAGE_TYPE="zip"
		if "INSTALLATION_FOLDER" in defines: self.INSTALL_PREFIX=defines["INSTALLATION_FOLDER"]
		elif self.HOST_OPERATING_SYSTEM=="Linux": self.INSTALL_PREFIX=GNU_INSTALLATION_FOLDER
		elif self.HOST_OPERATING_SYSTEM=="Windows": self.INSTALL_PREFIX=WINDOWS_INSTALLATION_FOLDER
		self.C_COMPILE_FLAGS=["-fPIC", "-fms-extensions"]
		self.CFLAGS=["-O", "-s", "-Iinclude"]
		self.LDFLAGS=["-O", "-s"]
		packages=Dependencies(packages=["libmodularis0", "libmodularis-dev", "libmodularis-cpp-dev"])
		Package(self, "build", "all").dependencies["build"]=packages
		Package(self, "install", "all").dependencies["install"]=packages
		Package(self, "remove", "all").dependencies["remove"]=packages
		Gzip(self, "debian/changelog", "out/share/doc")
		Clean(self, "all").dependencies["clean"]=packages
		self.BIN="out/bin"
		self.LIB="out/lib"
		if self.TARGET_OPERATING_SYSTEM=="Linux": Package(self, "build", "libmodularis0", {
			"build": Dependencies(
				[
					self.shared_library("modularis", "0"),
					"out/share/doc/changelog.gz"
				]
			)
		})
		elif self.TARGET_OPERATING_SYSTEM=="Windows": Package(self, "build", "libmodularis0", {
			"build": Dependencies(
				[self.shared_library("modularis", "0")]
			)
		})
		self.SRC="src/libmodularis0"
		self.OBJ="out/obj/libmodularis0"
		if self.TARGET_OPERATING_SYSTEM=="Linux": Shared_library_C(self, "modularis", "Note_chorus.o", "Transpose.o", "Amplifier.o", "Delay.o", "Modulator.o", "Sequencer.o", "Oscillator.o", "Sampler.o", "ADSR.o", "Integer_controller.o", "Real_controller.o", "Note.o", "Ports_folder.o", "Sound.o", "Pressed_oscillations.o", "Released_oscillations.o", "Pressed_samples.o", "Released_samples.o", "Sample.o", "Module.o", "Output.o", "Port.o", "Pattern_none.o", "Pattern.o", "Modularis.o", revision="0", ldflags=["-lm"])
		elif self.TARGET_OPERATING_SYSTEM=="Windows": Shared_library_C(self, "modularis", "Note_chorus.o", "Transpose.o", "Amplifier.o", "Delay.o", "Modulator.o", "Sequencer.o", "Oscillator.o", "Sampler.o", "ADSR.o", "Integer_controller.o", "Real_controller.o", "Note.o", "Ports_folder.o", "Sound.o", "Pressed_oscillations.o", "Released_oscillations.o", "Pressed_samples.o", "Released_samples.o", "Sample.o", "Module.o", "Output.o", "Port.o", "Pattern_none.o", "Pattern.o", "Modularis.o", revision="0")
		Object_C(self, "modules/effects/note/Note_chorus.c")
		Object_C(self, "modules/effects/note/Transpose.c")
		Object_C(self, "modules/effects/sound/Amplifier.c")
		Object_C(self, "modules/effects/sound/Delay.c")
		Object_C(self, "modules/effects/sound/Modulator.c")
		Object_C(self, "modules/players/Sequencer.c")
		Object_C(self, "modules/synthesizers/Oscillator.c")
		Object_C(self, "modules/synthesizers/Sampler.c")
		Object_C(self, "ports/controllers/ADSR.c")
		Object_C(self, "ports/controllers/Integer_controller.c")
		Object_C(self, "ports/controllers/Real_controller.c")
		Object_C(self, "ports/Note.c")
		Object_C(self, "ports/Ports_folder.c")
		Object_C(self, "ports/Sound.c")
		Object_C(self, "system/modules/synthesizers/Oscillator/Pressed_oscillations.c")
		Object_C(self, "system/modules/synthesizers/Oscillator/Released_oscillations.c")
		Object_C(self, "system/modules/synthesizers/Sampler/Pressed_samples.c")
		Object_C(self, "system/modules/synthesizers/Sampler/Released_samples.c")
		Object_C(self, "system/modules/synthesizers/Sampler/Sample.c")
		Object_C(self, "system/modules/Module.c")
		Object_C(self, "system/modules/Output.c")
		Object_C(self, "system/ports/Port.c")
		Object_C(self, "user/modules/players/Sequencer/Pattern_none.c")
		Object_C(self, "user/modules/players/Sequencer/Pattern.c")
		Object_C(self, "Modularis.c")
		Clean(self, "libmodularis0")
		if self.TARGET_OPERATING_SYSTEM=="Linux":
			Package(self, "build", "libmodularis-dev", {
				"build": Dependencies(
					[
						self.shared_library("modularis"),
						"out/share/doc/changelog.gz"
					]
				)
			})
			Symbolic_link(self, "libmodularis.so.0", self.LIB, "libmodularis.so")
		elif self.TARGET_OPERATING_SYSTEM=="Windows":
			Package(self, "build", "libmodularis-dev", {
				"build": Dependencies(
					[self.static_library("modularis.dll")]
				)
			})
			Static_DLL_interface(self, "modularis.dll", "libmodularis-0.dll")
		Clean(self, "libmodularis-dev")
		if self.TARGET_OPERATING_SYSTEM=="Linux": Package(self, "build", "libmodularis-cpp-dev", {
			"build": Dependencies(
				[
					self.static_library("modularis-cpp"),
					"out/share/doc/changelog.gz"
				]
			)
		})
		elif self.TARGET_OPERATING_SYSTEM=="Windows": Package(self, "build", "libmodularis-cpp-dev", {
			"build": Dependencies(
				[self.static_library("modularis-cpp")]
			)
		})
		self.SRC="src/libmodularis-cpp-dev"
		self.OBJ="out/obj/libmodularis-cpp-dev"
		Static_library(self, "modularis-cpp", "Note_chorus.o", "Transpose.o", "Amplifier.o", "Delay.o", "Modulator.o", "Sequencer.o", "Oscillator.o", "Sampler.o", "ADSR.o", "Integer_controller.o", "Real_controller.o", "Note.o", "Ports_folder.o", "Sound.o", "Pressed_oscillations.o", "Released_oscillations.o", "Pressed_samples.o", "Released_samples.o", "Sample.o", "Module.o", "Output.o", "Port.o", "Pattern_none.o", "Pattern.o", "Modularis.o")
		Object_Cpp(self, "modules/effects/note/Note_chorus.cpp")
		Object_Cpp(self, "modules/effects/note/Transpose.cpp")
		Object_Cpp(self, "modules/effects/sound/Amplifier.cpp")
		Object_Cpp(self, "modules/effects/sound/Delay.cpp")
		Object_Cpp(self, "modules/effects/sound/Modulator.cpp")
		Object_Cpp(self, "modules/players/Sequencer.cpp")
		Object_Cpp(self, "modules/synthesizers/Oscillator.cpp")
		Object_Cpp(self, "modules/synthesizers/Sampler.cpp")
		Object_Cpp(self, "ports/controllers/ADSR.cpp")
		Object_Cpp(self, "ports/controllers/Integer_controller.cpp")
		Object_Cpp(self, "ports/controllers/Real_controller.cpp")
		Object_Cpp(self, "ports/Note.cpp")
		Object_Cpp(self, "ports/Ports_folder.cpp")
		Object_Cpp(self, "ports/Sound.cpp")
		Object_Cpp(self, "system/modules/synthesizers/Oscillator/Pressed_oscillations.cpp")
		Object_Cpp(self, "system/modules/synthesizers/Oscillator/Released_oscillations.cpp")
		Object_Cpp(self, "system/modules/synthesizers/Sampler/Pressed_samples.cpp")
		Object_Cpp(self, "system/modules/synthesizers/Sampler/Released_samples.cpp")
		Object_Cpp(self, "system/modules/synthesizers/Sampler/Sample.cpp")
		Object_Cpp(self, "system/modules/Module.cpp")
		Object_Cpp(self, "system/modules/Output.cpp")
		Object_Cpp(self, "system/ports/Port.cpp")
		Object_Cpp(self, "user/modules/players/Sequencer/Pattern_none.cpp")
		Object_Cpp(self, "user/modules/players/Sequencer/Pattern.cpp")
		Object_Cpp(self, "Modularis.cpp")
		Clean(self, "libmodularis-cpp-dev")
		if self.TARGET_OPERATING_SYSTEM=="Linux":
			if self.pack and PACKAGE_TYPE=="deb":
				Package(self, "install", "libmodularis0", {
					"install": Dependencies(
						[self.install(join("lib", MULTIARCH, basename(self.shared_library("modularis", "0"))))]
					)
				})
				Install(self, self.shared_library("modularis", "0"), join("lib", MULTIARCH))
			else:
				Package(self, "install", "libmodularis0", {
					"install": Dependencies(
						[
							self.install(join("lib", basename(self.shared_library("modularis", "0")))),
							self.install("share/doc/libmodularis0/changelog.gz"),
							self.install("share/doc/libmodularis0/copyright")
						]
					)
				})
				Install(self, self.shared_library("modularis", "0"), "lib")
				Install(self, "out/share/doc/changelog.gz", "share/doc/libmodularis0")
		elif self.TARGET_OPERATING_SYSTEM=="Windows":
			Package(self, "install", "libmodularis0", {
				"install": Dependencies(
					[
						self.install(join("bin", basename(self.shared_library("modularis", "0")))),
						self.install("share/doc/libmodularis0/changelog"),
						self.install("share/doc/libmodularis0/copyright")
					]
				)
			})
			Install(self, self.shared_library("modularis", "0"), "bin")
			Install(self, "debian/changelog", "share/doc/libmodularis0")
		Install(self, "debian/copyright", "share/doc/libmodularis0")
		Remove(self, "libmodularis0")
		if self.TARGET_OPERATING_SYSTEM=="Linux":
			if self.pack and PACKAGE_TYPE=="deb":
				Package(self, "install", "libmodularis-dev", {
					"install": Dependencies(
						[
							self.install(join("lib", MULTIARCH, basename(self.shared_library("modularis")))),
							self.install("include/Modularis_Core")
						],
						packages=["libmodularis0"]
					)
				})
				Install(self, self.shared_library("modularis"), join("lib", MULTIARCH))
			else:
				Package(self, "install", "libmodularis-dev", {
					"install": Dependencies(
						[
							self.install(join("lib", basename(self.shared_library("modularis")))),
							self.install("include/Modularis_Core"),
							self.install("share/doc/libmodularis-dev/changelog.gz"),
							self.install("share/doc/libmodularis-dev/copyright")
						],
						packages=["libmodularis0"]
					)
				})
				Install(self, self.shared_library("modularis"), "lib")
				Install(self, "out/share/doc/changelog.gz", "share/doc/libmodularis-dev")
		elif self.TARGET_OPERATING_SYSTEM=="Windows":
			Package(self, "install", "libmodularis-dev", {
				"install": Dependencies(
					[
						self.install(join("lib", basename(self.static_library("modularis.dll")))),
						self.install("include/Modularis_Core"),
						self.install("share/doc/libmodularis-dev/changelog"),
						self.install("share/doc/libmodularis-dev/copyright")
					],
					packages=["libmodularis0"]
				)
			})
			Install(self, self.static_library("modularis.dll"), "lib")
			Install(self, "debian/changelog", "share/doc/libmodularis-dev")
		Install(self, "include/Modularis_Core", "include")
		Install(self, "debian/copyright", "share/doc/libmodularis-dev")
		Remove(self, "libmodularis-dev")
		if self.TARGET_OPERATING_SYSTEM=="Linux":
			if self.pack and PACKAGE_TYPE=="deb":
				Package(self, "install", "libmodularis-cpp-dev", {
					"install": Dependencies(
						[
							self.install(join("lib", MULTIARCH, basename(self.static_library("modularis-cpp")))),
							self.install("include/Modularis_Core_C++")
						],
						packages=["libmodularis0", "libmodularis-dev"]
					)
				})
				Install(self, self.static_library("modularis-cpp"), join("lib", MULTIARCH))
			else:
				Package(self, "install", "libmodularis-cpp-dev", {
					"install": Dependencies(
						[
							self.install(join("lib", basename(self.static_library("modularis-cpp")))),
							self.install("include/Modularis_Core_C++"),
							self.install("share/doc/libmodularis-cpp-dev/changelog.gz"),
							self.install("share/doc/libmodularis-cpp-dev/copyright")
						],
						packages=["libmodularis0", "libmodularis-dev"]
					)
				})
				Install(self, self.static_library("modularis-cpp"), "lib")
				Install(self, "out/share/doc/changelog.gz", "share/doc/libmodularis-cpp-dev")
		elif self.TARGET_OPERATING_SYSTEM=="Windows":
			Package(self, "install", "libmodularis-cpp-dev", {
				"install": Dependencies(
					[
						self.install(join("lib", basename(self.static_library("modularis-cpp")))),
						self.install("include/Modularis_Core_C++"),
						self.install("share/doc/libmodularis-cpp-dev/changelog"),
						self.install("share/doc/libmodularis-cpp-dev/copyright")
					],
					packages=["libmodularis0", "libmodularis-dev"]
				)
			})
			Install(self, self.static_library("modularis-cpp"), "lib")
			Install(self, "debian/changelog", "share/doc/libmodularis-cpp-dev")
		Install(self, "include/Modularis_Core_C++", "include")
		Install(self, "debian/copyright", "share/doc/libmodularis-cpp-dev")
		Remove(self, "libmodularis-cpp-dev")
		if PACKAGE_TYPE=="deb":
			Package(self, "pack", "all").dependencies["pack"]=packages
			Package(self, "pack", "libmodularis0").dependencies["pack"]=Dependencies([self.deb("libmodularis0", "0.0.0pre-alpha")])
			Package(self, "pack", "libmodularis-dev").dependencies["pack"]=Dependencies([self.deb("libmodularis-dev", "0.0.0pre-alpha")])
			Package(self, "pack", "libmodularis-cpp-dev").dependencies["pack"]=Dependencies([self.deb("libmodularis-cpp-dev", "0.0.0pre-alpha")])
			Deb(self, "libmodularis0", "0.0.0pre-alpha")
			Deb(self, "libmodularis-dev", "0.0.0pre-alpha")
			Deb(self, "libmodularis-cpp-dev", "0.0.0pre-alpha")
		else:
			Pack_archive(self, "all", "libmodularis_0.0.0pre-alpha_"+MULTIARCH+"."+PACKAGE_TYPE, "libmodularis0", "libmodularis-dev", "libmodularis-cpp-dev")
			Pack_archive(self, "libmodularis0", "libmodularis0_0.0.0pre-alpha_"+MULTIARCH+"."+PACKAGE_TYPE)
			Pack_archive(self, "libmodularis-dev", "libmodularis-dev_0.0.0pre-alpha_"+MULTIARCH+"."+PACKAGE_TYPE)
			Pack_archive(self, "libmodularis-cpp-dev", "libmodularis-cpp-dev_0.0.0pre-alpha_"+MULTIARCH+"."+PACKAGE_TYPE)
class Gzip(Path):
	def __init__(self, host, path, folder=None, name=None):
		if name==None: name=basename(path)+".gz"
		if folder==None: name=join(dirname(path), name)
		else: name=join(folder, name)
		super().__init__(host, host.ACTION_BUILD, name, {host.ACTION_BUILD: Dependencies([path], [] if len(dirname(name))==0 else [dirname(name)])})
	def recipe(self):
		if self.host.VERBOSE: print("  Make Gzip file \"", self.target, "\".", sep="")
		try:
			source=open(self.dependencies[self.action].updatable[0], "rb")
			target=gzip.open(self.target, "wb")
			target.write(source.read())
			source.close()
			target.close()
		except: return 1
		return 0
if __name__=="__main__": Modularis_Core("all", "build", "package").make()