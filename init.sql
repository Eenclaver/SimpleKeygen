-- init.sql
CREATE TABLE IF NOT EXISTS licenses (
    id SERIAL PRIMARY KEY,
    key VARCHAR(255) UNIQUE NOT NULL,
    product_id VARCHAR(100) NOT NULL,
    user_id VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMP NOT NULL,
    is_active BOOLEAN DEFAULT true,
    activation_count INT DEFAULT 0,
    last_activation TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_license_key ON licenses(key);
CREATE INDEX IF NOT EXISTS idx_product_user ON licenses(product_id, user_id);
CREATE INDEX IF NOT EXISTS idx_expires ON licenses(expires_at);

-- Можно добавить тестовые данные
INSERT INTO licenses (key, product_id, user_id, expires_at, is_active) 
VALUES ('TEST-KEY-1234-5678-9012', 'my_app_pro', 'test_user', '2024-12-31 23:59:59', true)
ON CONFLICT (key) DO NOTHING;