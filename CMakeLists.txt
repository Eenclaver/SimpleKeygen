cmake_minimum_required(VERSION 3.12)
project(LicenseServer)

set(CMAKE_CXX_STANDARD 17)

find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS uuid)

# PostgreSQL
find_path(PQXX_INCLUDE_DIR pqxx)
find_library(PQXX_LIBRARY pqxx)

# Redis
find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h)
find_library(HIREDIS_LIBRARY hiredis)

add_executable(license_server main.cpp)

target_include_directories(license_server PRIVATE
    ${PQXX_INCLUDE_DIR}
    ${HIREDIS_INCLUDE_DIR}
)

target_link_libraries(license_server PRIVATE
    cpprestsdk::cpprest
    OpenSSL::SSL
    OpenSSL::Crypto
    Boost::uuid
    ${PQXX_LIBRARY}
    ${HIREDIS_LIBRARY}
)