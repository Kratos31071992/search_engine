cmake_minimum_required(VERSION 3.10)
project(search_engine VERSION 0.1 LANGUAGES CXX)

# Настройка стандарта C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Подключение библиотеки JSON
find_package(nlohmann_json 3.2.0 REQUIRED)

# Настройка структуры проекта
set(SOURCE_DIR src)
set(INCLUDE_DIR include)

# Добавление исполняемого файла
add_executable(search_engine
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/ConverterJSON.cpp
    ${SOURCE_DIR}/InvertedIndex.cpp
    ${SOURCE_DIR}/SearchServer.cpp
)

# Подключение заголовочных файлов
target_include_directories(search_engine PRIVATE ${INCLUDE_DIR})

# Подключение библиотек
target_link_libraries(search_engine PRIVATE 
    nlohmann_json::nlohmann_json
)

# Настройка тестов (Google Test)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Добавление тестов
    add_executable(tests
        ${SOURCE_DIR}/InvertedIndex.cpp
        ${SOURCE_DIR}/SearchServer.cpp
        tests/test_invertedindex.cpp
        tests/test_searchserver.cpp
    )
    target_include_directories(tests PRIVATE ${INCLUDE_DIR})
    target_link_libraries(tests PRIVATE
        gtest_main
        nlohmann_json::nlohmann_json
    )
    enable_testing()
    add_test(NAME tests COMMAND tests)
endif()

# Копирование ресурсных файлов в build-директорию
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.json
      ${CMAKE_CURRENT_SOURCE_DIR}/requests.json
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Создание директории resources
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/resources)
