# TypingTest
Typing test desktop program with a large amount of customization

## Synopsis
TypingTest is a program to test your typing ability and allows you to practice
to improve it. It was created to have an open-source and native alternative to
10FastFingers.com as a school project. It seeks to improve upon the features of
10FastFingers by adding customization and features.

English language files taken from
https://github.com/first20hours/google-10000-english

## Insallation
### Dependencies
TypingTest is written in C++ with the gtkmm library. The runtime dependencies
are as follows:

- gtkmm 3.20 or higher.

The build dependencies are:

- A C++11 compiler, GCC and Clang both work.
- CMake 2.6 or higher.
- The `xdg-desktop-menu executable`.
- The `glib-compile-resources command`.

### Compilation
TypingTest uses CMake. Navigate into the build directory. TypingTest supports
in-tree builds. Run `cmake /path/to/source/dir` and `make install`.

### Configuration
You can configure the test settings through the settings menu.

TypingTest supports three major test types and a custom type. The basic test is
a short and quick test using basic words. The advanced test uses less common
words. The endurance test is meant to test your typing for extended periods of
time. The custom test allows you to tweak various settings such as how long the
test should be and what amount of the most common words should be included in
the test.

There are advanced settings as well. Many of these configure how your trouble
words are calculated.
