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

#include "popup_menu.h"

namespace typingtest {

PopupMenu *PopupMenu::create(Gtk::Window &parent)
{
    PopupMenu *menu = Gtk::manage(new PopupMenu(parent));
    return menu;
}

PopupMenu::PopupMenu(Gtk::Window &parent)
{
    accelerate(parent);
}

void PopupMenu::addItem(const std::string &name,
    const sigc::slot<void, Gtk::TreeRowReference> &slot)
{
    items.push_back({name, slot});
}

void PopupMenu::run(Gtk::TreeRowReference rowRef, guint button,
    guint32 activateTime)
{
    for (auto &item : items) {
        Gtk::MenuItem *menuItem = Gtk::manage(new Gtk::MenuItem(item.first));
        menuItem->signal_activate().connect(
            sigc::bind<Gtk::TreeRowReference>(item.second, rowRef));
        append(*menuItem);
    }
    show_all();
    popup(button, activateTime);
}

void PopupMenu::set_manage() {
    Gtk::Menu::set_manage();
}
} // typingtest
