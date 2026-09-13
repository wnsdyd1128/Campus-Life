cmake_minimum_required(VERSION 3.20)

project(CampusLife LANGUAGES CXX)

add_executable(CampusLife
    include/Course.hpp
    include/Student.hpp
    src/main.cpp
    src/Course.cpp
    src/Student.cpp
)

target_include_directories(CampusLife PRIVATE include)
target_compile_features(CampusLife PRIVATE cxx_std_20)

set_target_properties(CampusLife PROPERTIES
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(CampusLife PRIVATE /W4 /permissive-)
else()
    target_compile_options(CampusLife PRIVATE -Wall -Wextra -Wpedantic)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    src/main.cpp
    src/Course.cpp
    src/Student.cpp
    include/Course.hpp
    include/Student.hpp
)

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    PROPERTY VS_STARTUP_PROJECT CampusLife
)
