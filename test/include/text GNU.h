/*
(C) 2023-2024 Серый MLGamer. Все свободы предоставлены.
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

#define DESCRIPTION \
	"Серый MLGamer - Модулярис тест (0.0.0pre-alpha ремикс)\n"\
	"Это музыкальное произведение написано на языке программирования C на основе новейшей версии свободного каркаса Модулярис Ядро - 0.0.0pre-alpha! Это часть серии ремиксов \"гимна\" тестирования Модуляриса.\n"\
	"Наконец-то я доработал функционал каркаса до того, что с её помощью теперь возможно сделать какую-то стоящую музыку. Но, конечно, есть ещё к чему стремиться. Недостаток функционала синтеза звука скомпенсирован сэмплами. Вообще хотел сделать из этого \"гимна\" что-то типа House-попсни, но опять же не хватило звуков, поэтому Happy Hardcore и мешанина звуков, чтобы базовые волноформы сильно не выпирали.\n"
#if defined(NOGUI)
	#define HELP \
		DESCRIPTION\
		"Использование: modularis-test [опции]\n"\
		"Команда без опций \"--help\" и \"--output\" запустит проигрывание произведения.\n"\
		"Опции.\n"\
		"               --help                 Показать справку (это сообщение).\n"\
		"  -b <число>   --buffer=<число>       Установить размер аудиобуфера в фреймах. Значение по-умолчанию: 1024.\n"\
		"  -o <строка>  --output=<строка>      Срендерить произведение и записать в указанный файл в формате WAVE.\n"\
		"  -r <число>   --sample-rate=<число>  Установить частоту дискретизации в Герцах. Значение по-умолчанию: 44100.\n"
	#define OPPORTUNITY \
		"Попробуйте графический интерфейс! Вы можете скомпилировать проигрыватель с графическим интерфейсом, введя команду:\n"\
		"$ ./test\n"
#else
	#define HELP \
		DESCRIPTION\
		"Использование: modularis-test [опции]\n"\
		"Команда без опций \"--help\", \"--nogui\" и \"--output\" запустит проигрывание произведения в графическом режиме.\n"\
		"Опции.\n"\
		"               --help                     Показать справку (это сообщение).\n"\
		"  -b <число>   --buffer=<число>           Установить размер аудиобуфера в фреймах. Значение по-умолчанию: 1024.\n"\
		"               --nogui                    Запуск проигрывателя в консольном режиме.\n"\
		"  -o <строка>  --output=<строка>          Срендерить произведение и записать в указанный файл в формате WAVE.\n"\
		"  -r <число>   --sample-rate=<число>      Установить частоту дискретизации в Герцах. Значение по-умолчанию: 44100.\n"\
		"  -v <число>   --visualizer-time=<число>  Установить размер буфера осциллографа в миллисекундах. Значение по-умолчанию: 20.\n"
	#define OPPORTUNITY
#endif
#define PLAYER_TEXT \
	DESCRIPTION\
	"Если трек проигрывается прерывисто, купите новый компьютер. Либо примите участие в оптимизации производительности каркаса на GitHub: https://github.com/Seriy-MLGamer/Modularis_Core\n"\
	OPPORTUNITY\
	"Чтобы выйти из программы, нажмите любую букву, затем нажмите Enter.\n"\
	"Чтобы срендерить произведение и записать его в файл формата WAVE, запустите эту программу следующей командой:\n"\
	"$ ./modularis-test -o <выходной файл>"
#define WARNING_UNKNOWN_OPTION "modularis-test: предупреждение: неизвестная опция: \"%s\".\n"