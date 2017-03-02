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

#ifndef STICKER_BUFFER_H
#define STICKER_BUFFER_H

#include <string>
#include <utility>
#include <map>

#include <gtkmm.h>

#include "sticker_engine.h"

namespace typingtest {

// From the gtkmm documentation. This character is returned from the
// TextIter::get_char() function when it's on a pixbuf.
const gunichar UNKNOWN_CHAR = 0xFFFC;

class StickerBuffer : public Gtk::TextBuffer {
public:
	StickerBuffer();

	// Returns the text just as get_text would. However, a string of the form
	// :sticker_name:. Carelessly manipulating and reinserting this text may
	// result in unexpected results, as if there is an added colon then the
	// stickers will not display correctly. Because of the way text insertion
	// works, though, reinserting text retreived with this method will display
	// correctly because as of now there is now way create colons that would
	// mess this up.
	std::string getTextWithStickers();

	static Glib::RefPtr<StickerBuffer> create();

private:
	Glib::RefPtr<Gtk::TextTag> tag;
	void onInsertText(const Gtk::TextIter &iter, const Glib::ustring &text,
		int bytes);

	std::map<Glib::RefPtr<Gtk::TextTag>, int> stickerTags;

	StickerEngine engine;

	std::vector<std::pair<int, int>> splitChars(
		const std::vector<gunichar> &elements);
	// Pass by copy because the list is adjusted as we go. Shouldn't incur that
	// much overhead.
	void replaceWords(std::vector<std::pair<int, int>> words);
};
} // namespace typingtest

#endif // STICKER_BUFFER_H
