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

#include "sticker_dialog.h"

namespace typingtest {

const char *STALLMAN_STICKERS[] = {
	"stallman1",
	"stallman2"
};

const char *OTHER_STICKERS[] = {
};

StickerDialog::StickerDialog(Gtk::Window &parent)
	: Gtk::Dialog("Insert Sticker", parent, Gtk::DIALOG_MODAL),
	  modelNameColumns{100, RefPtrHasher<Gtk::TreeModel>{}}
{
	std::vector<std::string> stallmanStickers;
	for (const auto &name : STALLMAN_STICKERS)
		stallmanStickers.push_back(name);
	addCategory("Stallman", stallmanStickers);
	std::vector<std::string> otherStickers;
	for (const auto &name : OTHER_STICKERS)
		stallmanStickers.push_back(name);
	addCategory("Other", otherStickers);

	get_content_area()->pack_start(stickerNotebook, true, true);

	this->set_default_size(640, 480);
	show_all_children();

	this->add_button("OK", Gtk::RESPONSE_OK);
	this->add_button("Cancel", Gtk::RESPONSE_CANCEL);
}

void StickerDialog::onRowActivated(const Gtk::TreePath &path,
	Gtk::TreeViewColumn *column)
{
	Gtk::TreeView *view = column->get_tree_view();
	auto model = view->get_model();
	auto nameColumn = modelNameColumns[model];

	Gtk::TreeRow row = *model->get_iter(path);
}

void StickerDialog::addCategory(const std::string &name,
	std::vector<std::string> &stickerNames)
{
	std::shared_ptr<StickerCategory> category{new StickerCategory(name,
		stickerNames)};
	categories.push_back(category);
	stickerNotebook.append_page(category->viewScrolledWindow, category->name);
	category->view.signal_row_activated().connect(sigc::mem_fun(*this,
			&StickerDialog::onRowActivated));
	modelNameColumns[category->model] = &category->nameColumn;
}

StickerCategory::StickerCategory(const std::string &name,
	const std::vector<std::string> &stickerNames) : name{name}
{
	record.add(nameColumn);
	record.add(previewColumn);
	model = Gtk::ListStore::create(record);
	view.set_model(model);
	view.append_column("Name", nameColumn);
	view.append_column("Preview", previewColumn);
	viewScrolledWindow.add(view);

	for (const auto &name : stickerNames) {
		Gtk::TreeRow row{*model->append()};
		row[nameColumn] = name;
		row[previewColumn] = engine.createPixbufDefaultSize(name);
	}
}
} // namespace typingtest
