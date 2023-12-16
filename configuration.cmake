#(C) 2023 Серый MLGamer. All freedoms preserved.
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

#Target operating system and architecture signature.
#Possible values:
#  * host - for host system default signature;
#  * GNU/Linux:
#      - x86_64-linux-gnu;
#      - i386-linux-gnu;
#      - i686-linux-gnu;
#      - aarch64-linux-gnu;
#      - arm-linux-gnu;
#      - arm-linux-gnueabi;
#      - arm-linux-gnueabihf;
#  * Windows:
#      - x86_64-w64-mingw32;
#      - i686-w64-mingw32.
set(BUILD_TARGET host)
#Target type of package CPack will create from build.
#Possible values:
#  * host - for host system default package type (tar.gz for GNU and zip for Windows);
#  * tar.gz;
#  * zip;
#  * deb.
set(PACKAGE_TYPE host)
#The C/C++ compiler.
#Possible values:
#  * GCC.
#Comment the line below for a host default compiler.
set(COMPILER GCC)
set(GNU_INSTALLATION_FOLDER /usr/local)
set(WINDOWS_INSTALLATION_FOLDER "C:/Program Files/Modularis Core")