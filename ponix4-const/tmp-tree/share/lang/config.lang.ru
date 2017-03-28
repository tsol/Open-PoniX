# UTF-8
# CONFIGURATION menu specific texts. When you write configuration menu
# (such as /etc/xdmenu/net_menu.xd - you have to manually load the file,
# by calling "load_lang config" from your .xd file
#

# config menu

# session editor
LNG_VAR_EDIT_INPUT_TITLE="Введите значение"
LNG_VAR_EDIT_SELECT_TITLE="Выберите значение"

LNG_MENU_SESSION_EDITOR_TITLE="Настройка сессий"
LNG_MENU_SESSION_EDITOR_TEXT="Выберите сессию для правки:"

LNG_MENU_SE_DISABLED="- ОТКЛЮЧЕНО -"
LNG_MENU_SE_TITLE="Настройка сессии"
LNG_MENU_SE_TEXT="Параметры сессии"
LNG_MENU_SE_KILL="Завершить сессию"

LNG_MENU_SE_TYPE="Тип сессии"
LNG_MENU_SE_SES_TITLE="Название сессии"
LNG_MENU_SE_AUTOSTART="Автозапуск"
LNG_MENU_SE_TYPE_TEXT="Выберите тип сессии:"
LNG_MENU_SE_TITLE_TEXT="Введите имя сессии:"
LNG_MENU_SE_AUTOSTART_TEXT="Выберите будет ли сессия автоматически запущена после загрузки терминала:"

# resolution
LNG_MENU_RESOLUTION_TITLE="Разрешение экрана"
LNG_MENU_RESOLUTION_TEXT="Пожалуйста, выберите желаемое разрешение экрана:"
LNG_MENU_RESOLUTION_CURRENT="Активное"
LNG_MENU_RESOLUTION_SAVE_CHANGE="Разрешение экрана было изменено. Сохранить эти настройки?"
LNG_MENU_RESOLUTION_CHANGE_FAILED="Ошибка смены разрешения экрана! Этот видеорежим не поддерживается Вашей видеокартой."

# sound store
LNG_MENU_SOUNDSTORE_TITLE="Сохранение настроек звука"
LNG_MENU_SOUNDSTORE_TEXT="Как Вы хотите сохранить звуковые настройки?"
LNG_MENU_SOUNDSTORE_PXEMAC="Только для этого терминала (TFTP)"
LNG_MENU_SOUNDSTORE_PXEALL="Для всех терминалов с таким же железом (TFTP)"
LNG_MENU_SOUNDSTORE_LOCAL="Сохранить на локальный носитель"

LNG_SOUNDCFG_MIXER_TITLE="Настройка звуковой карты"
LNG_SOUNDCFG_ERR_NO_SOUND="Звуковая карта не найдена!"
LNG_SOUNDSTORE_NO_STORAGE="Терминал загружен по сети, нет доступного локального носителя!"
LNG_SOUNDSTORE_STORED="Настройки Вашей звуковой карты успешно СОХРАНЕНЫ!"

LNG_MENU_CONFIG_TITLE="Меню настроек"
LNG_MENU_CONFIG_TEXT="Меню настроек"
LNG_MENU_CONFIG_RESOLUTION="Разрешение экрана"
LNG_MENU_CONFIG_SESSIONS="Настройка сессий"
LNG_MENU_CONFIG_SOUND="Настройка звука"
LNG_MENU_CONFIG_NET="Настройка сети"
LNG_MENU_CONFIG_EDITFILE="Редактировать файл конфигурации"
LNG_MENU_CONFIG_UPLOADLOGS="Сохранить логи"

# network menu

LNG_MENU_NET_APPLY="Применить изменения"

LNG_MENU_NET_TITLE="Настройка сети"
LNG_MENU_NET_TEXT="Настройте Вашу сетевую карту:"

LNG_MENU_NET_CURRENT_IP="Текущий IP адрес"

LNG_MENU_NET_IFACE="	Текущий интерфейс"
LNG_MENU_NET_MODE="	IP режим"
LNG_MENU_NET_IPADDR="	IP адрес"
LNG_MENU_NET_MASK="	Маска сети"
LNG_MENU_NET_GW="	Адрес шлюза"
LNG_MENU_NET_DNS1=" 	Сервер DNS1"
LNG_MENU_NET_WIFI="	Настройки WiFi"
LNG_MENU_NET_SHOWCFG="	Посмотреть текущие настройки"

LNG_MENU_NET_SHOWCFG_TITLE="Текущие настройки сети"
LNG_MENU_NET_SHOWCFG_TEXT="Здесь показаны текущие сетевые настройки Вашего терминала:"
LNG_MENU_NET_SHOWCFG_MAC="MAC адрес"
LNG_MENU_NET_SHOWCFG_IFACE="Интерфейс"
LNG_MENU_NET_SHOWCFG_MODE="IP режим"
LNG_MENU_NET_SHOWCFG_IPADDR="IP адрес"
LNG_MENU_NET_SHOWCFG_MASK="Маска"
LNG_MENU_NET_SHOWCFG_GW="Шлюз"
LNG_MENU_NET_SHOWCFG_DNS1="Сервер DNS1"
LNG_MENU_NET_SHOWCFG_DNS2="Сервер DNS2"

# network menu submenus

LNG_MENU_NET_SELECT_DEVICE_TITLE="Выбор сетевого устройства"
LNG_MENU_NET_SELECT_DEVICE_TEXT="Пожалуйста, выберите какое сетевое устройство будет использоваться:"

LNG_MENU_NET_SELECT_MODE_TITLE="Выбор IP режима"
LNG_MENU_NET_SELECT_MODE_TEXT="Пожалуйста, выберите, какой метод получения IP адреса будет использоваться:"
LNG_MENU_NET_MODE_STATIC="Статический"
LNG_MENU_NET_MODE_DYNAMIC="DHCP"

LNG_MENU_NET_INPUT_IP_TITLE="Ввод IP адреса"
LNG_MENU_NET_INPUT_IP_TEXT="Пожалуйста, введите IP адрес, который будет использоваться на Вашем терминале:"

LNG_MENU_NET_INPUT_MASK_TITLE="Ввод маски сети"
LNG_MENU_NET_INPUT_MASK_TEXT="Пожалуйста, введите маску сети:"

LNG_MENU_NET_INPUT_GW_TITLE="Ввод адреса шлюза"
LNG_MENU_NET_INPUT_GW_TEXT="Пожалуйста, введите адрес шлюза, который будет использоваться на Вашем терминале:"

LNG_MENU_NET_INPUT_DNS1_TITLE="Ввод IP DNS сервера"
LNG_MENU_NET_INPUT_DNS1_TEXT="Пожалуйста, введите IP адрес DNS сервера, который будет использоваться на Вашем терминале:"

LNG_MENU_NET_RESTART_TITLE="Применение настроек сети"
LNG_MENU_NET_RESTART_TEXT="Применение настроек сети..."

# Wireless

LNG_MENU_NET_WIRELESS_TITLE="Настройки беспроводной сети"
LNG_MENU_NET_WIRELESS_TEXT="Настройте параметры Вашей беспроводной сети:"

LNG_MENU_NET_WIRELESS_ESSID_EMPTY="Введите имя беспроводной сети (ESSID)"
LNG_MENU_NET_WIRELESS_ESSID="Имя беспроводной сети (ESSID)"
LNG_MENU_NET_WIRELESS_KEY="Зашифрованный WPA2 PSK ключ"
LNG_MENU_NET_WIRELESS_IMPORT_FROM_STORAGE="Импортировать PSK ключ с локального носителя (key.txt)"

LNG_MENU_NET_WIRELESS_INPUT_ESSID_TITLE="Имя беспроводной сети"
LNG_MENU_NET_WIRELESS_INPUT_ESSID_TEXT="Пожалуйста, введите имя беспроводной сети (ESSID):"

LNG_MENU_NET_WIRELESS_INPUT_PSK_TITLE="Ключ PSK беспроводной сети"
LNG_MENU_NET_WIRELESS_INPUT_PSK_TEXT="Пожалуйста, введите PSK ключ для Вашей беспроводной сети:"

LNG_MENU_NET_WIRELESS_IMPORTKEY_STORAGE_FAIL="PSK ключ НЕ ИМПОРТИРОВАН! На локальном носителе не найден файл key.txt!"
LNG_MENU_NET_WIRELESS_IMPORTKEY_STORAGE_SUCCESS="Ключ успешно импортирован!"
LNG_MENU_NET_WIRELESS_STOREPSK_SHORT="PSK ключ слишком короткий! Ключ должен содержать, как минимум, 8 символов!"