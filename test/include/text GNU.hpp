/*
(C) 2023 Серый MLGamer. Все свободы предоставлены.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

Этот файл - часть каркаса Модулярис Ядро.
Модулярис Ядро - свободное ПО: вы можете перераспространять его и/или изменять его на условиях Стандартной общественной лицензии GNU в том виде, в каком она была опубликована Фондом свободного программного обеспечения; либо версии 3 лицензии, либо (по вашему выбору) любой более поздней версии.
ПО Модулярис Ядро распространяется в надежде, что оно будет полезно, но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной общественной лицензии GNU.
Вы должны были получить копию Стандартной общественной лицензии GNU вместе с этим ПО. Если это не так, см. <https://www.gnu.org/licenses/>.
*/

#define description \
"Серый MLGamer - Модулярис тест\n"\
"Это музыкальное произведение написано на языке программирования C++ на основе моего свободного каркаса Модулярис Ядро самой первой версии - 0.0.0pre-alpha-raw-undone!\n"
#define help \
description\
"Использование: modularis-test [опции]\n"\
"Команда без опций \"--help\" и \"--output\" запустит проигрывание произведения.\n"\
"Опции.\n"\
"                              --help                                 Показать справку (это сообщение).\n"\
"  -o <выходной файл>          --output=<выходной файл>               Срендерить произведение и записать в указанный файл в формате WAVE.\n"\
"  -r <частота дискретизации>  --sample-rate=<частота дискретизации>  Установить частоту дискретизации в Герцах. Значение по-умолчанию: 44100.\n"
#define player_text \
description\
"\n"\
"Чтобы выйти из программы, нажмите любую букву, затем нажмите Enter.\n"\
"Чтобы срендерить произведение и записать его в файл формата WAVE, запустите эту программу следующей командой: modularis-test -o <выходной файл>"
#define warning_unknown_option "modularis-test: ошибка: неизвестная опция: \""