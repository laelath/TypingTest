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

#include <cstdlib>

#include <chrono>
#include <fstream>
#include <iostream>

#include <pangomm/fontdescription.h>

#include <gtkmm.h>

#include "config.h"
#include "typing_test_window.h"

int main(int argc, char *argv[])
{
	auto app = Gtk::Application::create(argc, argv, "us.laelath.typingtest");
	// Create test
	try {
		auto builder = Gtk::Builder::create_from_resource(
			"/us/laelath/typingtest/ui/typingui.glade");
		typingtest::TypingTestWindow *window = nullptr;
		builder->get_widget_derived("typingtest", window);
		int status = app->run(*window);
		delete window;
		return status;
    } catch (const Glib::FileError e) {
        std::cerr << e.what() << std::endl;
    } catch (const Gio::ResourceError& e) {
        std::cerr << e.what() << std::endl;
    } catch (const Gtk::BuilderError& e) {
        std::cerr << e.what() << std::endl;
    }

	return EXIT_FAILURE;
}
