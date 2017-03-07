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

#ifndef TYPING_TEST_STICKER_DIALOG_H
#define TYPING_TEST_STICKER_DIALOG_H

#include <unordered_map>
#include <memory>

#include <gtkmm.h>

#include "sticker_engine.h"

namespace typingtest {

extern const char *STALLMAN_STICKERS[];
extern const char *OTHER_STICKERS[];

template<typename T>
class RefPtrHasher {
public:
	size_t operator()(const Glib::RefPtr<T> &) const;
};

// A set of widgets and information that are meant to be stored in a notebook
// for a StickerDialog. They contain the necessary information for creating a
// page and filling it with stickers.
struct StickerCategory {
	StickerCategory(const std::string &name,
		const std::vector<std::string> &stickerNames);

	std::string name;
	Gtk::ScrolledWindow viewScrolledWindow;
	Gtk::TreeView view;
	Glib::RefPtr<Gtk::ListStore> model;
	Gtk::TreeModelColumnRecord record;
	Gtk::TreeModelColumn<Glib::ustring> nameColumn;
	Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> previewColumn;

	StickerEngine engine;
};

// A dialog that is means for selecting a sticker and inserting it. Has a
// Notebook of sticker categories and a sticker can be selected by double
// clicking on one of them. This class is meant to be used by running the
// dialog, checking the hasSticker() function, and then getting the sticker
// name with getStickerName();
class StickerDialog : public Gtk::Dialog {
public:
	StickerDialog();
	StickerDialog(Gtk::Window &parent);

	bool hasSticker() const;
	std::string getStickerName() const;

private:
	Gtk::Notebook stickerNotebook;
	std::vector<std::shared_ptr<StickerCategory>> categories;

	bool sticker = false;
	std::string stickerName;

	Glib::RefPtr<Gtk::ListStore> allStickersStore;
	Gtk::TreeModelColumn<Glib::ustring> allStickersColumn;

	void addCategory(const std::string &name,
		std::vector<std::string> &stickerNames);

	// Map for associating models with the first column in that model. This is
	// required because in one of the signal handlers, the model can be
	// obtained but the corresponding name columns are necessary.
	std::unordered_map<Glib::RefPtr<Gtk::TreeModel>,
		Gtk::TreeModelColumn<Glib::ustring> *, RefPtrHasher<Gtk::TreeModel>>
			modelNameColumns;

	void onRowActivated(const Gtk::TreePath &path, Gtk::TreeViewColumn *column);

	StickerEngine engine;
};

template<typename T>
size_t
RefPtrHasher<T>::operator()(const Glib::RefPtr<T> &) const
{
	return 0;
};
} // namespace typingtest

#endif // TYPING_TEST_STICKER_DIALOG_H
