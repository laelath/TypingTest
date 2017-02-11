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

StickerDialog::StickerDialog(Gtk::Window &parent)
	: Gtk::Dialog("Insert Sticker", parent, Gtk::DIALOG_MODAL)
{
	searchEntry.set_completion(entryCompletion);
	get_content_area()->pack_start(searchEntry, false, false);

	stallmanRecord.add(stallmanColumn);
	stallmanStore = Gtk::ListStore::create(stallmanRecord);
	stallmanView.set_model(stallmanStore);
	stallmanView.append_column("Name", stallmanColumn);
	for (const auto &stickerName : STALLMAN_STICKERS) {
		Gtk::TreeIter iter{stallmanStore->append()};
		Gtk::TreeRow row{*iter};
		row[stallmanColumn] = stickerName;
		/* Gtk::TreeIter allIter{allStickersStore->append()}; */
		/* Gtk::TreeRow allRow{*allIter}; */
		/* allRow[allStickersColumn] = stickerName; */
	}
	stickerNotebook.append_page(stallmanView, "Stallman");

	otherRecord.add(otherColumn);
	otherStore = Gtk::ListStore::create(otherRecord);
	otherView.set_model(otherStore);
	stickerNotebook.append_page(otherView, "Other");

	get_content_area()->pack_start(stickerNotebook, true, true);

	this->set_default_size(640, 480);
	show_all_children();

	this->add_button("OK", Gtk::RESPONSE_OK);
	this->add_button("Cancel", Gtk::RESPONSE_CANCEL);

}
} // namespace typingtest
