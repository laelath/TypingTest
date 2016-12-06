cmake_minimum_required(VERSION 2.6)
project(typing-test)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
add_definitions(${GTKMM_CFLAGS_OTHER})

set(SOURCES src/main.cpp src/typing_test.cpp)

add_executable(typingtest ${SOURCES})

target_link_libraries(typingtest ${GTKMM_LIBRARIES})
