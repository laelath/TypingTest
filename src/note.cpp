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
// TypingTest.  If not, see <http://www.gnu.org/licenses/>.

#include "note.h"

#include <cstio>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <gtkmm.h>

namespace typingtest {

Note::Note(const std::string &name, const std::tm &time)
	: name{name}, time{time}
{
}

const std::string &Note::getName() const
{
	return name;
}

void Note::setName(const std::string &name)
{
	this->name = name;
}

const std::tm &Note::getTime() const
{
	return time;
}

void Note::setTime(const std::tm &time)
{
	this->time = time;
}

const std::string &Note::getContents() const
{
	return contents;
}

void Note::setContents(const std::string &contents)
{
	this->contents = contents;
}

bool Note::save(const std::string &dirPath) const
{
	if (name.length() == 0)
		return false;
	std::string filePath = dirPath + "/" + name;

	std::ofstream writer{filePath};
	if (!writer.is_open())
		return false;
	writer << name << std::endl;
	writer << getTimeString() << std::endl;
	writer << contents;
	return true;
}

std::string Note::getTimeString() const
{
	std::ostringstream timeString;
	timeString << std::put_time(&time, TIME_FORMAT);
	return timeString.str();
}

Note Note::readNote(const std::string &filePath)
{
	std::ifstream reader{filePath};
	if (!reader.is_open())
		throw std::runtime_error{"Failed to open note file " + filePath + "."};
	std::string name;
	std::getline(reader, name);
	std::tm time;
	std::string dateString;
	std::getline(reader, dateString);
	std::istringstream dateStream{dateString};
	dateStream >> std::get_time(&time, TIME_FORMAT);
	Note note{name, time};
	std::string contents{std::istreambuf_iterator<char>(reader),
		std::istreambuf_iterator<char>()};;
	note.setContents(contents);
	return note;
}

std::tm currentTime()
{
	time_t now = time(NULL);
	return *std::localtime(&now);
}

std::string Note::uniqueNoteName(const std::string &noteName,
	const std::string &noteDir)
{
	std::string tmpName{noteName};
	std::string path = noteDir + "/" + tmpName;
	int tries = 0;
	while (Glib::file_test(path, Glib::FILE_TEST_EXISTS)) {
		++tries;
		if (tries >= MAX_SCORE_WITH_SAME_NAME)
			throw std::runtime_error{"Failed to find unique note name."};
		tmpName = noteName + "(" + std::to_string(tries) + ")";
		path = noteDir + "/" + tmpName;
	}
	return tmpName;
}

bool Note::deleteNote(const std::string &name, const std::string &noteDir)
{
	std::string path = noteDir + "/" + name;
	return std::remove(path.c_str()) == 0;
}
} // namespace typingtest
