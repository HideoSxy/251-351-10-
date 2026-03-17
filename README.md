# StegoApp

TCP-сервер на Qt/C++ с SQLite базой данных.  

## Docker Desktop — запуск сервера в контейнере

1. Регистрируемся на [docker.com](https://www.docker.com/)

2. Установить Docker Desktop — [docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop/)

3. Скачать и запустить сервер:
```bash
docker run -d -p 33333:33333 --name stegoapp dimkk8/stegoapp-server:latest
```

## Проверить что сервер работает

### Через PuTTY

1. Открыть PuTTY
2. Настройки:
   - **Host Name:** `127.0.0.1`
   - **Port:** `33333`
   - **Connection type:** `Raw`
3. Нажать **Соединиться**
4. В терминале появится:
   ```
   CONNECTED: StegoApp Server ready. Use reg& or auth& to start.
   ```

### Команды для проверки


```
# Регистрация
reg&login,password

# Авторизация
auth&login,password

# SHA-384 хэш
sha384&hello

# Заглушки (отвечают ERR: stub)
rsa_gen
chord&0,2,0.0001
# И другие
# Эти функции находятся в разработке
```

## Что нужно установить

### Windows

1. **QTCreator, QTFramework (version 4.5+)** — скачать с [qt.io/download](https://www.qt.io/development/download-qt-installer-oss)  
   При установке нужно выбрать:
   - `Qt 6.10 для desktop разработки`
   - `MinGW`
   - `CMake

2. **CMake** — установится вместе с Qt.

3. **Git** — [git-scm.com](https://git-scm.com/install/)

### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y \
    git \
    build-essential \
    cmake \
    qtbase5-dev \
    libqt5sql5-sqlite \
    libqt5network5
```

---

## Клонировать и собрать

### Linux / Mac

```bash
# 1. Клонировать репозиторий
git clone https://github.com/HideoSxy/251-351-10-
cd https://github.com/HideoSxy/251-351-10-/backend_server

# 2. Создать папку сборки
mkdir build
cd build

# 3. Сгенерировать Makefile
cmake ..

# 4. Собрать (параллельно по числу ядер)
make -j$(nproc)

# 5. Запустить сервер
./main_server
```

### Windows (через Qt Creator)

1. Открыть **Qt Creator**
2. `File` → `Open File or Project`
3. Выбрать файл `backend_server/CMakeLists.txt`
4. Qt Creator настроит проект
5. Нажать кнопку **Run** или `Ctrl+R`

### Windows (через командную строку)

```bat
git clone https://github.com/HideoSxy/251-351-10-
cd https://github.com/HideoSxy/251-351-10-\backend_server

mkdir build
cd build

:: Путь к cmake может отличаться
cmake .. -G "MinGW Makefiles"

mingw32-make -j4

main_server.exe
```

---


## Структура проекта

```
backend_server/
├── CMakeLists.txt          ← сборка
├── main_server.cpp         ← точка входа
├── mytcpserver.h/.cpp      ← TCP-сервер, буфер клиентов, роли
├── requesthandler.h/.cpp   ← парсинг команд, маршрутизация
├── functionstoserver.h/.cpp ← бизнес-логика (reg, auth, sha384, заглушки)
└── databasemanager.h/.cpp  ← SQLite singleton (создаёт stego_app.db)
```

**В будущем:** После первого запуска в папке `build/` появится файл `stego_app.db` — это база данных SQLite со всеми пользователями.

---

