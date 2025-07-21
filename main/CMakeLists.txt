# Основная программа
add_executable(search_engine
    src/ConverterJSON.cpp
    src/InvertedIndex.cpp
    src/SearchServer.cpp
    src/main.cpp
)

target_include_directories(search_engine PRIVATE include)

# Подключение JSON библиотеки
find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

# Копирование ресурсов в build-директорию
file(COPY resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(COPY ../test_data/config.json ../test_data/requests.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
