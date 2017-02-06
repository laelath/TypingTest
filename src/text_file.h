// Copyright (C) 2017 Justin Frank, Jason Waataja
//
// This file is part of TypingTest.
//
// TypingTest is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// TypingTest is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// TypingTest.  If not, see <http:// www.gnu.org/licenses/>.

#ifndef TEXT_FILE_H
#define TEXT_FILE_H

#include <iostream>
#include <fstream>
#include <string>

namespace typingtest {

// TextFile represents a file on the computer that can be read from and written
// to. When opened, it creates a swap file and on a save renames the swap file
// to the name of the intended file.
class TextFile {
public:
	TextFile();
	TextFile(const std::string &path);
	~TextFile();

	// Attemps to open the given file for reading or writing. If another file
	// was open, closes the streams without saving and deletes the swap file.
	bool openFile(const std::string& path);

	// Returns the path represented by the file.
	const std::string &getPath() const;
	// Returns the path currently being used for the swap file.
	const std::string &getSwapPath() const;

	// Returns if the file has reading capabilities.
	bool hasOpenReader() const;
	// Returns if the file has writing capabilities.
	bool hasOpenWriter() const;
	// Flushes the swap file and renames it to the main file. The TextFile
	// object is in an invalid state after this and should not be written to or
	// read from unless openFile is called.
	//
	// Throws a runtime_error with the given errno string on failure.
	void save();
	// Closes both the reader and the writer. Don't attempt to read or write
	// after this unless openFile is called.
	void close();

	// Implicit conversion to boolean that returns true if both the reader and
	// writer are open.
	operator bool() const;

	// For easy writing.
	template <typename T>
	friend std::ostream& operator<<(TextFile &file, const T &object);
	// For easy reading.
	template <typename T>
	friend std::istream& operator>>(TextFile &file, const T &object);

private:
	std::string path;
	std::string swapPath;

	// Reads the file given by path.
	std::ifstream reader;
	// Writes to the file given by swapPath.
	std::ofstream writer;

	// Remove the swap file. Throws a runtime_error if it fails with the given
	// errno string.
	void removeSwapFile();

	// Checks for a file with the given path with ".swp" appended. If such a
	// file already exists then it tries appending the number 1 to the end.
	// Continues to do this until a file that doesn't already exists is found.
	//
	// Tries opening 20 files. If a suitable file could not be found then it
	// throws a runtime_exception.
	std::string getSwapFileName(const std::string &path);
};

// For easy writing.
template <typename T>
std::ostream &operator<<(TextFile &file, const T &object);

// For easy reading.
template <typename T>
std::istream &operator>>(TextFile &file, const T &object);
} // namespace typingtest

#endif // TEXT_FILE_H
