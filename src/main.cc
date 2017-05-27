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
#include <iostream>

#include <gtkmm.h>

#include "typing_test_application.h"

int main(int argc, char *argv[])
{
    auto app = typingtest::TypingTestApplication::create(argc, argv,
        "us.laelath.typingtest");
    try {
        int status = app->run();
        return status;
    } catch (const Glib::FileError &e) {
	std::cerr << e.what() << std::endl;
    } catch (const Gio::ResourceError &e) {
	std::cerr << e.what() << std::endl;
    } catch (const Gtk::BuilderError &e) {
	std::cerr << e.what() << std::endl;
    } catch (...) {
	std::cerr << "Unknown exception" << std::endl;
    }
    return EXIT_FAILURE;
}