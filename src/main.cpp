#include <chrono>
#include <iostream>

#include <pangomm/fontdescription.h>

#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>
#include <gtkmm/entry.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#include "typing_test.h"

int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "us.laelath.typingtest");

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
	builder->add_from_file("ui/typingui.glade");

	Gtk::ApplicationWindow *appWindow;
	Gtk::Entry *entry;
	Gtk::Label *label;
	Gtk::ImageMenuItem *quit;
	Gtk::TextView *textView;

	builder->get_widget("typingtest", appWindow);
	builder->get_widget("typingentry", entry);
	builder->get_widget("timerlabel", label);
	builder->get_widget("quit", quit);
	builder->get_widget("textview", textView);

	entry->override_font(Pango::FontDescription("15"));
	label->override_font(Pango::FontDescription("15"));
	textView->override_font(Pango::FontDescription("30"));

	Glib::RefPtr<Gtk::TextBuffer> textBuffer =
		Glib::RefPtr<Gtk::TextBuffer>::cast_static(builder->get_object("textbuffer"));
	Glib::RefPtr<Gtk::TextTagTable> textTags =
		Glib::RefPtr<Gtk::TextTagTable>::cast_static(builder->get_object("texttags"));

	Glib::RefPtr<Gtk::TextTag> currentTag =
		Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("currenttag"));
	Glib::RefPtr<Gtk::TextTag> errorTag =
		Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("errortag"));
	Glib::RefPtr<Gtk::TextTag> goodTag =
		Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("goodtag"));

	textTags->add(currentTag);
	textTags->add(errorTag);
	textTags->add(goodTag);

	Glib::RefPtr<Gtk::EntryBuffer> entryBuffer =
		Glib::RefPtr<Gtk::EntryBuffer>::cast_static(builder->get_object("typingbuffer"));

	TypingTest test(textBuffer, entryBuffer, label, 200, 2, 100, std::chrono::seconds(60),
			std::chrono::system_clock::now().time_since_epoch().count());

	quit->signal_activate().connect(sigc::mem_fun(appWindow, &Gtk::ApplicationWindow::close));

	return app->run(*appWindow);
}
