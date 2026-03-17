# Берём Ubuntu 22.04 как основу
FROM ubuntu:22.04

# Отключаем интерактивные вопросы при установке пакетов
ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем Qt5, CMake и компилятор
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qtbase5-dev \
    libqt5sql5-sqlite \
    libqt5network5 \
    && rm -rf /var/lib/apt/lists/*

# Копируем все исходники в контейнер в папку /app
WORKDIR /app
COPY . .

# Собираем проект
RUN mkdir build && cd build && cmake .. && make -j$(nproc)

# Говорим Docker что сервер слушает порт 33333
EXPOSE 33333

# Команда запуска при старте контейнера
CMD ["./build/main_server"]
