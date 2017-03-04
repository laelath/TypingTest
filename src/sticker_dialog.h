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

#endif // STICKER_DIALOG_H
