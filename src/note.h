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

const char TIME_FORMAT[] = "%Y-%m-%d %H:%M:%S";

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

private:
	std::string name;
	std::tm time;
	std::string contents;
};
} // namespace typingtest

#endif // NOTE_H
