# Этап 1: сборка проекта
FROM ubuntu:22.04 AS builder

# Отключаем интерактивные вопросы при установке пакетов
ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем инструменты для сборки
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qtbase5-dev \
    libqt5sql5-sqlite \
    libqt5network5 \
    && rm -rf /var/lib/apt/lists/*

# Копируем исходники и собираем проект
WORKDIR /app
COPY . .
RUN mkdir build && cd build && cmake .. && make -j$(nproc)

# Этап 2: создание минимального рабочего образа
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libqt5core5a \
    libqt5network5 \
    libqt5sql5 \
    libqt5sql5-sqlite \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/build/main_server /app/main_server

WORKDIR /app

EXPOSE 33333

CMD ["./main_server"]
