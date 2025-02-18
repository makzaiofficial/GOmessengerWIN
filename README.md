# GOmessengerWIN
GOmessenger WIN64

Инструкции по сборке для 64-разрядной Windows
Подготовьте папку
Установите программное обеспечение сторонних производителей
Скопируйте исходный код и подготовьте библиотеки
Создайте проект
Инструменты Qt Visual Studio
Подготовьте папку
Сборка выполняется в Visual Studio 2022 с версией SDK 10.0.22000.0.

Выберите пустую папку для будущей сборки, например D:\TBuild. В остальной части этого документа она будет называться BuildPath. Создайте там две папки: BuildPath\ThirdParty и BuildPath\Libraries.

Все команды (если не указано иное) будут запускаться из командной строки x64 Native Tools для VS 2022.bat (должна быть в меню Пуск > папка меню Visual Studio 2022). Обратите внимание, что не следует использовать какую-либо другую командную строку.

Получите свои учетные данные API
Для доступа к серверам Telegram API вам потребуются api_id и api_hash. Чтобы узнать, как их получить, нажмите здесь.

Установите программное обеспечение сторонних производителей
Загрузите установщик Python 3.10 с сайта https://www.python.org/downloads/ и установите его, добавив в PATH.
Скачайте Git installer с сайта https://git-scm.com/download/win и установите его.
Скопируйте исходный код и подготовьте библиотеки
Откройте командную строку x64 Native Tools для VS 2022.bat, перейдите в BuildPath и запустите

git clone --recursive https://github.com/telegramdesktop/tdesktop.git
После распаковать и заменить директорию Telegram в корне tdesktop\
git clone --recursive [https://github.com/makzaiofficial/GOmessengerWIN/GOmessengerWIN.git](https://github.com/makzaiofficial/GOmessengerWIN.git)
tdesktop\GOmessenger\build\prepare\win.bat
Создайте проект
Перейдите в BuildPath\tdesktop\GOmessenger и запустите (используя свой api_id и api_hash)

configure.bat x64 -D TDESKTOP_API_ID=ВАШ_API_ID -D TDESKTOP_API_HASH=ВАШ_API_HASH
Откройте BuildPath\GOmessenger\out\Telegram.sln в Visual Studio 2022
Выберите проект Telegram и нажмите Build > Построить Telegram (Конфигурации отладки и выпуска).
Результат Telegram.exe будет размещен в D:\TBuild\GOmessenger\out\Debug (и выпущен)
Qt Visual Studio Tools
Для улучшения отладки вы можете установить Qt Visual Studio Tools:

Откройте Расширения -> Управление расширениями
Перейдите на вкладку Онлайн
Найдите Qt
Установите расширение Qt Visual Studio Tools
