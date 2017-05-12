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

#ifndef TYPING_TEST_POPUP_MENU_H
#define TYPING_TEST_POPUP_MENU_H

#include <gtkmm.h>

namespace typingtest {

// Represents a popup menu to be used with a TreeView. Add items with addItem
// that takes slots that use a TreeRowReference to do things. Run should be the
// only function you need to call on it as it does all the showing and
// processing.
class PopupMenu : private Gtk::Menu {
public:
    // Creates a popup menu managed by the parent. Don't worry about freeing
    // the return value for this function because it is managed by the parent.
    static PopupMenu *create(Gtk::Window &parent);

    // Adds an item with the given name that runs the given slot on a click.
    void addItem(const std::string &name,
        const sigc::slot<void, Gtk::TreeRowReference> &slot);
    // Calls show_all to make it display, and then calls "popup".
    void run(Gtk::TreeRowReference rowRef, guint button, guint32 activateTime);
    void set_manage() override;

private:
    PopupMenu(Gtk::Window &parent);

    std::vector<std::pair<std::string, sigc::slot<void, Gtk::TreeRowReference>>>
        items;
    void onItemActivate();
};
} // namespace typingtest

#endif // TYPING_TEST_POPUP_MENU_H
