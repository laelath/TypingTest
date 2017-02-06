cmake_minimum_required (VERSION 2.6)
project(TypingTest)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GTKMM REQUIRED gtkmm-3.0)

execute_process (
	COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=glib_compile_resources gio-2.0
	OUTPUT_VARIABLE GLIB_COMPILE_RESOURCES
	OUTPUT_STRIP_TRAILING_WHITESPACE)

add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.c
	DEPENDS res/typingtest.gresource.xml res/ui/typingui.glade res/ui/dice-icon-small.png
	COMMAND ${GLIB_COMPILE_RESOURCES} ${CMAKE_SOURCE_DIR}/res/typingtest.gresource.xml --target=${CMAKE_CURRENT_BINARY_DIR}/resources.c --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/res --generate-source)

include_directories (${GTKMM_INCLUDE_DIRS})
link_directories (${GTKMM_LIBRARY_DIRS})
add_definitions (${GTKMM_CFLAGS_OTHER})

set (
	SOURCES
	src/main.cpp
	src/typing_test.cpp
	src/typing_test_window.cpp
	src/test_settings.cpp
	src/word.cpp
	src/config.cpp
	src/test_info.cpp
	src/text_file.cpp
	${CMAKE_CURRENT_BINARY_DIR}/resources.c)

add_executable (typingtest ${SOURCES})

target_link_libraries (typingtest ${GTKMM_LIBRARIES})
set_property (TARGET typingtest PROPERTY CXX_STANDARD 11)

install (TARGETS typingtest DESTINATION bin)

file (COPY TypingTest.desktop DESTINATION ${CMAKE_BINARY_DIR})
install (FILES ${CMAKE_BINARY_DIR}/TypingTest.desktop DESTINATION share/applications)
