cmake_minimum_required(VERSION 3.21)

project(TemplateMetaProgrammingTests VERSION 0.0.1
        DESCRIPTION "Test for the tmp library"
        LANGUAGES CXX)


#find_package(tmp CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/algorithms.cpp
    ${CMAKE_CURRENT_LIST_DIR}/typelist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME}
    tmp
)
