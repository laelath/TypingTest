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

void testPrint(std::string in, int *num)
{
	std::cout << "entry added: " << in << std::endl;
}

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

	TypingTest test(TestType::BASIC, 200, 60, std::chrono::system_clock::now().time_since_epoch().count());
	textBuffer->set_text(test.getWords());
	label->set_text(test.getTime());

	//textBuffer->apply_tag(goodTag, textBuffer->get_iter_at_offset(0), textBuffer->get_iter_at_offset(4));
	//textBuffer->apply_tag(errorTag, textBuffer->get_iter_at_offset(8), textBuffer->get_iter_at_offset(12));
	//textBuffer->apply_tag(currentTag, textBuffer->get_iter_at_offset(54), textBuffer->get_iter_at_offset(64));
	
	entry->signal_insert_text().connect(sigc::ptr_fun(&testPrint));

	quit->signal_activate().connect(sigc::mem_fun(appWindow, &Gtk::ApplicationWindow::close));

	return app->run(*appWindow);
}
