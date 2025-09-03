cmake_minimum_required(VERSION 3.12)
project(LicenseServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# Ищем библиотеки вручную
find_path(CPPREST_INCLUDE_DIR cpprest/http_listener.h)
find_library(CPPREST_LIBRARY cpprest)

find_path(PQXX_INCLUDE_DIR libpqxx/libpqxx )
find_library(PQXX_LIBRARY libpqxx )

if (NOT PQXX_INCLUDE_DIR)
    set(PQXX_INCLUDE_DIR "/usr/include")
endif()

if (NOT PQXX_LIBRARY)
    set(PQXX_LIBRARY "/usr/lib/x86_64-linux-gnu/libpqxx.so")
endif()

message(STATUS "PQXX include dir: ${PQXX_INCLUDE_DIR}")
message(STATUS "PQXX library: ${PQXX_LIBRARY}")

find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h)
find_library(HIREDIS_LIBRARY hiredis)

find_package(OpenSSL REQUIRED)

# Ищем компоненты Boost по отдельности
find_path(BOOST_INCLUDE_DIR boost/uuid/uuid.hpp)
find_library(BOOST_SYSTEM_LIB boost_system)
find_library(BOOST_UUID_LIB boost_uuid)
find_library(BOOST_RANDOM_LIB boost_random)
find_library(BOOST_THREAD_LIB boost_thread)
find_library(BOOST_FILESYSTEM_LIB boost_filesystem)
find_library(BOOST_CHRONO_LIB boost_chrono)
find_library(BOOST_ATOMIC_LIB boost_atomic)
find_library(BOOST_DATE_TIME_LIB boost_date_time)
find_library(BOOST_REGEX_LIB boost_regex)
find_library(BOOST_PROGRAM_OPTIONS_LIB boost_program_options)


if (NOT BOOST_UUID_LIB)
    set(BOOST_UUID_LIB "/usr/lib/x86_64-linux-gnu/boostid.so")
endif()

message(STATUS "PQXX include dir: ${PQXX_INCLUDE_DIR}")
message(STATUS "PQXX library: ${PQXX_LIBRARY}")

if (NOT CPPREST_LIBRARY)
    message(FATAL_ERROR "cpprest library not found")
endif()

if (NOT PQXX_LIBRARY)
    message(FATAL_ERROR "pqxx library not found")
endif()

if (NOT HIREDIS_LIBRARY)
    message(FATAL_ERROR "hiredis library not found")
endif()

add_executable(license_server main.cpp)

target_include_directories(license_server PRIVATE
    ${CPPREST_INCLUDE_DIR}
    ${PQXX_INCLUDE_DIR}
    ${HIREDIS_INCLUDE_DIR}
    ${BOOST_INCLUDE_DIR}
)

target_link_libraries(license_server PRIVATE
    ${CPPREST_LIBRARY}
    ${PQXX_LIBRARY}
    ${HIREDIS_LIBRARY}
    ${BOOST_SYSTEM_LIB}
    ${BOOST_UUID_LIB}
    ${BOOST_RANDOM_LIB}
    ${BOOST_THREAD_LIB}
    ${BOOST_FILESYSTEM_LIB}
    ${BOOST_CHRONO_LIB}
    ${BOOST_ATOMIC_LIB}
    ${BOOST_DATE_TIME_LIB}
    ${BOOST_REGEX_LIB}
    ${BOOST_PROGRAM_OPTIONS_LIB}
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
    ssl
    crypto
)