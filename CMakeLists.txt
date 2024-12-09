cmake_minimum_required(VERSION 3.10)
project(NeverForgetBot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PostgreSQL REQUIRED)
find_package(CURL REQUIRED)
find_package(TgBot REQUIRED)

# Include directories
include_directories(${PostgreSQL_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(/usr/local/include) # For tgbot-cpp

# Add the jwt-cpp include directory. Adjust this path if needed.
# Assuming jwt-cpp headers are at: third-party/jwt-cpp/include/jwt-cpp/jwt.h
include_directories(${CMAKE_SOURCE_DIR}/third-party/jwt-cpp/include)

# Link directories
link_directories(${PostgreSQL_LIBRARY_DIRS})
link_directories(${CURL_LIBRARY_DIRS})
link_directories(/usr/local/lib) # For tgbot-cpp

add_executable(NeverForgetBot
    src/main.cpp
    src/db/db.cpp
    src/db/users.cpp
    src/db/event.cpp
    src/db/notification.cpp
    src/bot/commands/start.cpp
    src/bot/commands/help.cpp
    src/bot/commands/change_tz.cpp
    src/bot/commands/upcoming-events.cpp
    src/bot/handlers/callback/callback.cpp
    src/bot/handlers/callback/time-zone.cpp
    src/bot/handlers/callback/delay-notification.cpp
    src/bot/handlers/callback/change-event-status.cpp
    src/bot/utils/utils.cpp
    src/chrono/periodic_task.cpp
    src/parse_msg/parse.cpp
    src/parse_msg/generate_aim.cpp
    src/parse_msg/parse_request.cpp
    src/parse_msg/checker.cpp
    src/parse_msg/env_parser.cpp
    src/bot/handlers/notifications/notifications.cpp
    src/bot/handlers/events/events.cpp
)

target_link_libraries(NeverForgetBot
    TgBot
    pqxx
    pq
    pthread
    curl
    ssl
    crypto
)
