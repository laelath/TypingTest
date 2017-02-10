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

#include <gtkmm.h>

#include "sticker_engine.h"

namespace typingtest {

class StickerBuffer : public Gtk::TextBuffer {
public:
	StickerBuffer();

	static Glib::RefPtr<StickerBuffer> create();

private:
	void onInsertText(const Gtk::TextIter &iter, const Glib::ustring &text,
		int bytes);

	StickerEngine engine;
};
} // namespace typingtest

#endif // STICKER_BUFFER_H
