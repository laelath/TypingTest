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

#ifndef STICKER_ENGINE_H
#define STICKER_ENGINE_H

#include <string>

#include <gtkmm.h>

namespace typingtest {

const char STICKER_PREFIX[] = "/us/laelath/typingtest/stickers/";

class StickerEngine {
public:
	StickerEngine();
	~StickerEngine();

	std::string getStickerPath(const std::string &stickerName);

	// Returns true if there is a sticker with the given name available.
	bool hasSticker(const std::string &stickerName);

	// Creates a pixbuf for the given sticker name.
	//
	// Returns a smart pointer to the pixbuf if it could be created, a null
	// smart pointer otherwise.
	Glib::RefPtr<Gdk::Pixbuf> createPixbufForSticker(
		const std::string &stickerName);
};
} // namespace typingtest

#endif // STICKER_ENGINE_H
