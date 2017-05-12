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

#ifndef TYPING_TEST_TYPING_TEST_APPLICATION_H
#define TYPING_TEST_TYPING_TEST_APPLICATION_H

#include <vector>

#include <gtkmm.h>

namespace typingtest {

class TypingTestApplication : public Gtk::Application {
public:
    TypingTestApplication(int &argc, char **&argv,
        const Glib::ustring &applicationId = Glib::ustring{},
        Gio::ApplicationFlags flags = Gio::APPLICATION_FLAGS_NONE);
    static Glib::RefPtr<TypingTestApplication> create(int &argc, char **&argv,
        const Glib::ustring &applicationId = Glib::ustring{},
        Gio::ApplicationFlags flags = Gio::APPLICATION_FLAGS_NONE); 

private:
    std::vector<Gtk::Window *> windows;
    void on_activate() override;
    void onShutdown();
};
} // namespace typingtest

#endif // TYPING_TEST_TYPING_TEST_APPLICAITON_H
