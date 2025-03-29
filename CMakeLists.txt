cmake_minimum_required(VERSION 3.21)

project("TemplateMetaProgramming" VERSION 0.0.1
        DESCRIPTION "A header-only C++ library to learn Template Meta Programming"
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/alexanderwachter/TemplateMetaProgramming")

set(TARGET_NAME tmp)

add_library(${TARGET_NAME} INTERFACE)
target_compile_features(${TARGET_NAME} INTERFACE cxx_std_20)

target_include_directories(${TARGET_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

option(ENABLE_TESTING "Enable the tests" ${PROJECT_IS_TOP_LEVEL})
if(ENABLE_TESTING)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)
endif()
