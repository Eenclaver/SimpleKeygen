#include <iostream>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <pqxx/pqxx>
#include <hiredis/hiredis.h>
#include <openssl/sha.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

class LicenseServer {
private:
    http_listener listener;
    std::string db_connection_string;

    std::string generate_license_key(const std::string& product_id, const std::string& user_id) {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        std::string uuid_str = boost::uuids::to_string(uuid);
        
        // Создаем хеш на основе UUID, product_id и user_id
        std::string data = product_id + ":" + user_id + ":" + uuid_str;
        
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, data.c_str(), data.size());
        SHA256_Final(hash, &sha256);
        
        // Конвертируем в hex строку
        char hex_hash[65];
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            sprintf(hex_hash + (i * 2), "%02x", hash[i]);
        }
        hex_hash[64] = 0;
        
        // Берем первые 20 символов и форматируем
        std::string key = "KEY-";
        for(int i = 0; i < 20; i += 4) {
            key += std::string(hex_hash + i, 4);
            if(i < 16) key += "-";
        }
        
        return key;
    }

    void handle_generate(http_request request) {
        request.extract_json().then([=](json::value request_json) {
            try {
                std::string product_id = request_json.at("product_id").as_string();
                std::string user_id = request_json.at("user_id").as_string();
                std::string expires_at = request_json.at("expires_at").as_string();

                // Генерируем ключ
                std::string license_key = generate_license_key(product_id, user_id);

                // Сохраняем в PostgreSQL
                pqxx::connection conn(db_connection_string);
                pqxx::work txn(conn);
                txn.exec_params(
                    "INSERT INTO licenses (key, product_id, user_id, expires_at, is_active) "
                    "VALUES ($1, $2, $3, $4, true)",
                    license_key, product_id, user_id, expires_at
                );
                txn.commit();

                // Кэшируем в Redis
                redisContext *redis = redisConnect("redis", 6379);
                if (redis != NULL && !redis->err) {
                    std::string redis_key = "license:" + license_key;
                    redisCommand(redis, "SET %s %s EX 3600", 
                                redis_key.c_str(), "active");
                    redisFree(redis);
                }

                // Возвращаем ответ
                json::value response;
                response["license_key"] = json::value::string(license_key);
                response["status"] = json::value::string("created");

                request.reply(status_codes::OK, response);

            } catch (const std::exception& e) {
                json::value error;
                error["error"] = json::value::string(e.what());
                request.reply(status_codes::BadRequest, error);
            }
        });
    }

    void handle_validate(http_request request) {
        request.extract_json().then([=](json::value request_json) {
            try {
                std::string license_key = request_json.at("license_key").as_string();
                std::string product_id = request_json.at("product_id").as_string();

                // Сначала проверяем в Redis
                redisContext *redis = redisConnect("redis", 6379);
                if (redis != NULL && !redis->err) {
                    std::string redis_key = "license:" + license_key;
                    redisReply *reply = (redisReply*)redisCommand(
                        redis, "GET %s", redis_key.c_str());
                    
                    if (reply != NULL && reply->type == REDIS_REPLY_STRING) {
                        redisFree(redis);
                        
                        json::value response;
                        response["valid"] = json::value::boolean(true);
                        response["cached"] = json::value::boolean(true);
                        request.reply(status_codes::OK, response);
                        return;
                    }
                    redisFree(redis);
                }

                // Если нет в кэше, проверяем в PostgreSQL
                pqxx::connection conn(db_connection_string);
                pqxx::work txn(conn);
                pqxx::result result = txn.exec_params(
                    "SELECT expires_at, is_active FROM licenses "
                    "WHERE key = $1 AND product_id = $2",
                    license_key, product_id
                );

                json::value response;
                if (!result.empty()) {
                    auto row = result[0];
                    std::string expires_at = row["expires_at"].as<std::string>();
                    bool is_active = row["is_active"].as<bool>();

                    // Проверяем срок действия
                    // TODO: Добавить проверку даты

                    response["valid"] = json::value::boolean(is_active);
                    response["expires_at"] = json::value::string(expires_at);
                } else {
                    response["valid"] = json::value::boolean(false);
                }

                request.reply(status_codes::OK, response);

            } catch (const std::exception& e) {
                json::value error;
                error["error"] = json::value::string(e.what());
                request.reply(status_codes::BadRequest, error);
            }
        });
    }

public:
    LicenseServer(const std::string& url, const std::string& db_conn_str) 
        : listener(url), db_connection_string(db_conn_str) {
        
        listener.support(methods::POST, 
            [=](http_request request) {
                auto path = request.request_uri().path();
                if (path == "/KeygenApi/generate") {
                    handle_generate(request);
                } else if (path == "/KeygenApi/validate") {
                    handle_validate(request);
                } else {
                    request.reply(status_codes::NotFound);
                }
            });
    }

    pplx::task<void> open() { return listener.open(); }
    pplx::task<void> close() { return listener.close(); }
};