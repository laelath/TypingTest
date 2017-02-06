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

#include "text_file.h"

#include <sys/stat.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace typingtest {

TextFile::TextFile(const std::string &path)
	: path(path), swapPath(getSwapFileName(path))
{
	reader.open(path);
	writer.open(swapPath);
}

TextFile::TextFile()
{
}

bool TextFile::openFile(const std::string &path)
{
	close();
	try {
		removeSwapFile();
	} catch (std::runtime_error) {
		return false;
	}
	
	this->path = path;
	swapPath = getSwapFileName(path);
	reader.open(path);
	writer.open(swapPath);
	return (reader.is_open() && writer.is_open());
}

void TextFile::removeSwapFile()
{
	if (writer.is_open())
		writer.close();
	if (std::remove(swapPath.c_str()) != 0)
		throw std::runtime_error(std::strerror(errno));
}

TextFile::~TextFile()
{
	close();
	removeSwapFile();
}

std::string TextFile::getSwapFileName(const std::string &path)
{
	std::string swapPathTest = path + ".swp";
	struct stat pathInfo;
	if (stat(swapPathTest.c_str(), &pathInfo) != 0 && errno == ENOENT)
		return swapPathTest;
	for (int i = 0; i < 20; ++i) {
		swapPathTest = path + ".swp" + std::to_string(i);
		if (stat(swapPathTest.c_str(), &pathInfo) != 0 && errno == ENOENT)
			return swapPathTest;
	}
	throw std::runtime_error("Failed to find suitable swap file");
}

void TextFile::save()
{
	close();
	if (!std::rename(swapPath.c_str(), path.c_str()))
		throw std::runtime_error(std::strerror(errno));
}

bool TextFile::hasOpenReader() const
{
	return reader.is_open();
}

bool TextFile::hasOpenWriter() const
{
	return writer.is_open();
}

const std::string &TextFile::getPath() const
{
	return path;
}

const std::string &TextFile::getSwapPath() const
{
	return swapPath;
}

template <typename T>
std::ostream &operator<<(TextFile &file, const T &object)
{
	if (file.writer.is_open())
		file.writer << object;
	return file.writer;
}

template <typename T>
std::istream &operator>>(TextFile &file, const T &object)
{
	if (file.reader.is_open())
		file.reader >> object;
	return file.reader;
}

void TextFile::close()
{
	if (reader.is_open())
		reader.close();
	if (writer.is_open())
		writer.close();
}

TextFile::operator bool() const
{
	return reader.is_open() && writer.is_open();
}
} // namespace typingtest
