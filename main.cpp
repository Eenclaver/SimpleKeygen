#include "LicenseServer.cpp"

int main(){
  try {
        std::cout << "Initializing License Server..." << std::endl;
        
        // Получаем настройки из переменных окружения
        std::string db_host = get_env("DB_HOST", "db");
        std::string db_port = get_env("DB_PORT", "5432");
        std::string db_name = get_env("DB_NAME", "licenses");
        std::string db_user = get_env("DB_USER", "admin");
        std::string db_pass = get_env("DB_PASS", "secretpassword");
        
        std::string redis_host = get_env("REDIS_HOST", "redis");
        std::string redis_port_str = get_env("REDIS_PORT", "6379");
        int redis_port = std::stoi(redis_port_str);
        
        // Формируем строку подключения к БД
        std::string db_connection_string = 
            "postgresql://" + db_user + ":" + db_pass + "@" + 
            db_host + ":" + db_port + "/" + db_name;
        
        std::cout << "Database connection: " << db_connection_string << std::endl;
        std::cout << "Redis connection: " << redis_host << ":" << redis_port << std::endl;
        
        // Создаем и запускаем сервер
        LicenseServer server("http://0.0.0.0:8088", db_connection_string, redis_host, redis_port);
        
        std::cout << "Server created, opening..." << std::endl;
        server.open().wait();
        
        std::cout << "License Server is running on http://0.0.0.0:8088" << std::endl;
        std::cout << "Endpoints:" << std::endl;
        std::cout << "  POST /api/generate" << std::endl;
        std::cout << "  POST /api/validate" << std::endl;
        std::cout << "  GET  /api/health" << std::endl;
        std::cout << "Press Ctrl+C to stop..." << std::endl;
        
        // Бесконечный цикл чтобы сервер не завершался
        while(true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
      }
      catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
      }
    
    return 0;
}