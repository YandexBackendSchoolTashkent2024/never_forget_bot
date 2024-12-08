cmake_minimum_required(VERSION 3.10)
project(TelegramBot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PostgreSQL REQUIRED)
find_package(CURL REQUIRED)
find_package(TgBot REQUIRED)

# Include directories
include_directories(${PostgreSQL_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(/usr/local/include) # For tgbot-cpp

# Link directories
link_directories(${PostgreSQL_LIBRARY_DIRS})
link_directories(${CURL_LIBRARY_DIRS})
link_directories(/usr/local/lib) # For tgbot-cpp

add_executable(TelegramBot
    src/main.cpp
    src/db/db.cpp
    src/db/users.cpp
    src/bot/handlers/command_handlers.cpp
    src/bot/utils/utils.cpp
    src/chrono/periodic_task.cpp
    
)

target_link_libraries(TelegramBot
    TgBot
    pqxx
    pq
    pthread
    curl
    ssl
    crypto
)
