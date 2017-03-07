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

#ifndef NOTE_H
#define NOTE_H

#include <ctime>
#include <string>

namespace typingtest {

std::tm currentTime();

// The string to be used with std::get_time and std::put_time. Uses ISO 8601
// year and time formats. I tried using "%F %T" before but that didn't work
// since I don't think you're supposed to use those two together. Doing this
// manually works just as well.
const char TIME_FORMAT[] = "%Y-%m-%d %H:%M:%S";

// A class that represents a note taken by a user, storing its contents as well
// as some simple data. Supports simple saving, loading, and deleting. Also
// stores a time, defaults to the current time.
class Note {
public:
	Note(const std::string &name = {}, const std::tm &time = currentTime());

	const std::string &getName() const;
	void setName(const std::string &name);
	const std::tm &getTime() const;
	void setTime(const std::tm &tm);
	const std::string &getContents() const;
	void setContents(const std::string &contents);

	std::string getTimeString() const;
	// Reads the note stored in filePath and returns it. Throws a runtime_error if
	// it encounters an error.
	static Note readNote(const std::string &filePath);
	// Writes the contents of the note to a file with the note name in dirPath.
	//
	// Returns true on success, false on failure.
	bool save(const std::string &dirPath) const;
	// Returns true on success, false on failure.
	static bool deleteNote(const std::string &name, const std::string &noteDir);

	static const int MAX_SCORE_WITH_SAME_NAME = 99999;
	// Searches noteDir for existing files. Looks for paths that are not yet
	// taken that start with noteName. If a file already exists, appends (num)
	// to the end. If MAX_SCORE_WITH_SAME_NAME is reached, then a
	// runtime_error is thrown.
	static std::string uniqueNoteName(const std::string &noteName,
		const std::string &noteDir);

private:
	std::string name;
	std::tm time;
	std::string contents;
};
} // namespace typingtest

#endif // NOTE_H
