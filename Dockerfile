# Stage 1: Build stage
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    libpq-dev \
    libhiredis-dev \
    libssl-dev \
    libpqxx-dev \
    libboost-all-dev \
    libcpprest-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    strip license_server

# Stage 2: Runtime stage - Debian Slim
FROM debian:12-slim

# Устанавливаем только необходимые runtime пакеты
RUN apt-get update && apt-get install -y \
    libpqxx-dev \
    libpq5 \
    libcpprest-dev \
    libhiredis-dev \
    libssl-dev \
    libboost-all-dev \
    libhiredis0.14 \
    libssl3 \
    libcpprest2.10 \
    libboost-system1.74.0 \
    libboost-program-options1.74.0 \
    libboost-random1.74.0 \
    libboost-thread1.74.0 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Создаем пользователя
RUN groupadd -r appgroup && useradd -r -g appgroup appuser

WORKDIR /app
COPY --from=builder --chown=appuser:appgroup /build/build/license_server .

USER appuser

EXPOSE 8080
CMD ["./license_server"]