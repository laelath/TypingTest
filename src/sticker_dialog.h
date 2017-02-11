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

#ifndef STICKER_DIALOG_H
#define STICKER_DIALOG_H

#include <gtkmm.h>

namespace typingtest {

const std::string STALLMAN_STICKERS[] = {
	"stallman1",
};

class StickerDialog : public Gtk::Dialog {
public:
	StickerDialog();
	StickerDialog(Gtk::Window &parent);

private:
	Gtk::SearchEntry searchEntry;
	Glib::RefPtr<Gtk::EntryCompletion> entryCompletion;
	Gtk::Notebook stickerNotebook;
	Gtk::ScrolledWindow stickerScrolledWindow;
	Gtk::TreeView stallmanView;
	Gtk::TreeModelColumnRecord stallmanRecord;
	Gtk::TreeModelColumn<Glib::ustring> stallmanColumn;
	Glib::RefPtr<Gtk::ListStore> stallmanStore;
	Gtk::TreeView otherView;
	Gtk::TreeModelColumnRecord otherRecord;
	Gtk::TreeModelColumn<Glib::ustring> otherColumn;
	Glib::RefPtr<Gtk::ListStore> otherStore;

	Glib::RefPtr<Gtk::ListStore> allStickersStore;
	Gtk::TreeModelColumn<Glib::ustring> allStickersColumn;
};
} // namespace typingtest

#endif // STICKER_DIALOG_H
