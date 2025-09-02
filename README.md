# 🗝️ SimpleKeygen

Микросервис для генерации и валидации лицензионных ключей с использованием C++, PostgreSQL и Redis.

## 🚀 Быстрый старт

```bash
# Клонируйте и запустите
git clone <your-repo>
cd license-server
docker-compose up --build
```

# Сервер будет доступен на http://localhost:8088

# 📋 API Endpoints

1. 🆕 Генерация лицензионного ключа

POST /api/generate

Создает новый лицензионный ключ и сохраняет его в базу данных.

📥 Input:
```json
{
  "product_id": "my_app_pro",
  "user_id": "user_12345",
  "expires_at": "2024-12-31T23:59:59"
}
```

📤 Output (успех):
```json
{
  "license_key": "KEY-1A2B3C4D-5E6F7G8H-9I0J1K2L",
  "status": "created",
  "product_id": "my_app_pro",
  "expires_at": "2024-12-31T23:59:59"
}
```

📤 Output (ошибка):
```json
{
  "error": "Invalid product_id format"
}
```

2. ✅ Валидация лицензионного ключа
POST /api/validate

Проверяет валидность лицензионного ключа для указанного продукта.

📥 Input:
```json
{
  "license_key": "KEY-1A2B3C4D-5E6F7G8H-9I0J1K2L",
  "product_id": "my_app_pro"
}
```

📤 Output (валидный ключ):
```json
{
  "valid": true,
  "cached": false,
  "expires_at": "2024-12-31T23:59:59",
  "is_active": true
}
```

📤 Output (невалидный ключ):
```json
{
  "valid": false,
  "reason": "key_not_found"
}
```

3. 📊 Статус сервера

GET /api/health

Проверяет работоспособность сервера и подключения к БД.

📤 Output:
```json
{
  "status": "healthy",
  "database": "connected",
  "redis": "connected",
  "timestamp": "2024-01-15T10:30:00Z"
}
```

🏗️ Архитектура

┌─────────────────┐    HTTP    ┌─────────────────┐
│   Client App    │───────────▶│  License Server │
│                 │◀───────────│    (C++ REST)   │
└─────────────────┘            └─────────┬───────┘
                                         │
                                         │ DB Queries
                                         │
                         ┌───────────────┴───────────────┐
                         │                               │
                         ▼                               ▼
┌─────────────────┐            ┌─────────────────┐            ┌─────────────────┐
│   Redis Cache   │◀──────────│   PostgreSQL    │──────────▶ │   Data Storage  │
│   (кэш ключей)  │──────────▶│   (основная БД) │            │     (Volume)    │
└─────────────────┘            └─────────────────┘            └─────────────────┘

⚙️ Конфигурация

Environment Variables
Переменная	По умолчанию	Описание
DB_HOST	        db	        Хост PostgreSQL
DB_PORT	        5438	    Внешний порт PostgreSQL
DB_NAME	       changeme	    Имя базы данных
DB_USER	       changeme	    Пользователь БД
DB_PASS	       changeme	    Пароль БД
REDIS_HOST	    redis	    Хост Redis
REDIS_PORT	    6378	    Внешний порт Redis

Формат лицензионных ключей
Ключи генерируются в формате:
KEY-XXXX-XXXX-XXXX-XXXX-XXXX

Где:

KEY- - префикс

XXXX - 4 случайных hex-символа (0-9, A-F)

Всего 25 символов (включая дефисы)

📊 База данных

Структура таблицы licenses

Поле	            Тип	            Описание
id	                SERIAL	        Primary key
key	                VARCHAR(255)	Лицензионный ключ (уникальный)
product_id	        VARCHAR(100)	Идентификатор продукта
user_id	            VARCHAR(100)	Идентификатор пользователя
created_at	        TIMESTAMP	    Дата создания
expires_at	        TIMESTAMP	    Дата окончания действия
is_active	        BOOLEAN	        Активна ли лицензия
activation_count    INTEGER	        Количество активаций

🔧 Примеры использования

Генерация ключа:

```bash
curl -X POST http://localhost:8080/api/generate \
  -H "Content-Type: application/json" \
  -d '{
    "product_id": "photo_editor_pro",
    "user_id": "customer_789",
    "expires_at": "2024-12-31T23:59:59"
  }'
```

Проверка ключа:

```bash
curl -X POST http://localhost:8080/api/validate \
  -H "Content-Type: application/json" \
  -d '{
    "license_key": "KEY-1A2B3C4D-5E6F7G8H-9I0J1K2L",
    "product_id": "photo_editor_pro"
  }'
```

Проверка здоровья:

```bash
curl http://localhost:8080/api/health
```
# 🛡️ Безопасность

Все ключи хранятся в зашифрованной базе данных

Redis кэш автоматически очищается через 1 час

Поддержка HTTPS (нуждается в дополнительной настройке)

Валидация входных данных