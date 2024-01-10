@echo off

rem (C) 2023-2024 Серый MLGamer. All freedoms preserved.
rem Дзен: <https://dzen.ru/seriy_mlgamer>
rem SoundCloud: <https://soundcloud.com/seriy_mlgamer>
rem YouTube: <https://www.youtube.com/@Seriy_MLGamer>
rem GitHub: <https://github.com/Seriy-MLGamer>
rem E-mail: <Seriy-MLGamer@yandex.ru>
rem
rem This file is part of Modularis Core.
rem Modularis Core is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
rem Modularis Core is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
rem You should have received a copy of the GNU General Public License along with Modularis Core. If not, see <https://www.gnu.org/licenses/>.

set INSTALL_DIR=C:\Program Files\Modularis Core
gcc -omodularis-test main.c -O -s -Iinclude -I"%INSTALL_DIR%\include" -L"%INSTALL_DIR%\lib" -lmodularis.dll -lmingw32 -lSDL2main -lSDL2.dll
set OLD_PATH=%PATH%
set PATH=%OLD_PATH%;%INSTALL_DIR%\bin
modularis-test
set PATH=%OLD_PATH%