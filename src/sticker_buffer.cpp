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

#include "sticker_buffer.h"

#include <regex>

namespace typingtest {

StickerBuffer::StickerBuffer() : TextBuffer()
{
	signal_insert().connect(sigc::mem_fun(*this,
			&StickerBuffer::onInsertText));
}

void StickerBuffer::onInsertText(const Gtk::TextIter &pos,
	const Glib::ustring &text, int bytes)
{
	int posOffset = pos.get_offset();
	if (text.find(":") != Glib::ustring::npos) {
		std::string fullText{get_text()};
		std::regex re{R"(:(\w*):)"};
		std::smatch match;

		while (std::regex_search(fullText, match, re)) {
			Gtk::TextIter startIter{get_iter_at_offset(match.position())};
			Gtk::TextIter endIter{get_iter_at_offset(match.position()
				+ match.length())};
			erase(startIter, endIter);
			Glib::RefPtr<Gdk::Pixbuf> sticker =
				engine.createPixbufForSticker(match.str(1));
			if (sticker) {
				Gtk::TextIter pixbufIter = get_iter_at_offset(match.position());
				insert_pixbuf(pixbufIter, sticker);
			}
		}
	}
}

Glib::RefPtr<StickerBuffer> StickerBuffer::create()
{
	return Glib::RefPtr<StickerBuffer>(new StickerBuffer());
}
} // typingtest
