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

#include "typing_test_application.h"

#include "typing_test_window.h"

namespace typingtest {

TypingTestApplication::TypingTestApplication(int &argc, char **&argv,
	const Glib::ustring &applicationId, Gio::ApplicationFlags flags)
	: Gtk::Application{argc, argv, applicationId, flags}
{
	signal_shutdown().connect(sigc::mem_fun(*this,
			&TypingTestApplication::onShutdown));
}

Glib::RefPtr<TypingTestApplication> TypingTestApplication::create(int &argc,
	char **&argv, const Glib::ustring &applicationId,
	Gio::ApplicationFlags flags)
{
	return Glib::RefPtr<TypingTestApplication>{new TypingTestApplication{argc,
		argv, applicationId, flags}};
}

void TypingTestApplication::on_activate()
{
	Gtk::Application::on_activate();
	TypingTestWindow *window = TypingTestWindow::create_pointer();
	add_window(*window);
	window->show();
	windows.push_back(window);
}

void TypingTestApplication::onShutdown()
{
	for (Gtk::Window *window : windows)
		delete window;
}
} // namespace typingtest
