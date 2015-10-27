1. Для работы нужно:
 - Скопировать в эту папку исходник плана под именем "plan.bin"
 - Скопировать в эту папку файл прошивки контроллера под именем "edicapp-ng.bin"
 - Скопировать в эту папку файл рекофигурации сенсора под именем "sreconf.bin"
 - Запустить "make.bat"
 - Ввести серийный номер капсулы

2. При первом запуске на компьютере нужно
 - Открыть файл проекта под именем "edicfwdecompressor.uvprojx" в IDE Keil
 - Произвести сборку проекта (Project->Build Target)
 - Выполнить п.1

Утилиты, необходимые для работы скрипта:
 - lzma
 - datatoc
 - edicidgenerator
 - edickeygenerator
 - edickeycomposer
 - edicimage
 - edictool