# Тесты
enable_testing()

add_executable(tests
    src/test_invertedindex.cpp
    src/test_searchserver.cpp
)

target_include_directories(tests PRIVATE ../main/include)

# Подключение зависимостей
find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(tests PRIVATE nlohmann_json::nlohmann_json)

# Подключение GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)
target_link_libraries(tests PRIVATE gtest_main)

add_test(NAME search_engine_tests COMMAND tests)
