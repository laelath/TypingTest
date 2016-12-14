#include <chrono>
#include <iostream>

#include <pangomm/fontdescription.h>

#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#include "typing_test.h"

Gtk::ApplicationWindow *appWindow;
Gtk::Button *newTest;
Gtk::ImageMenuItem *quit;
Gtk::Box *testBox;

TestWidgets widgets;

TypingTest *test;

void genNewTest()
{
	delete test;

	test = new TypingTest(widgets, 200, 2, 100, std::chrono::seconds(60),
			std::chrono::system_clock::now().time_since_epoch().count());
}

int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "us.laelath.typingtest");

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
	builder->add_from_file("ui/typingui.glade");

	builder->get_widget("typingtest", appWindow);
	builder->get_widget("newtestbutton", newTest);
	builder->get_widget("quit", quit);
	builder->get_widget("testbox", testBox);

	builder->get_widget("textview", widgets.textView);
	builder->get_widget("typingentry", widgets.entry);
	builder->get_widget("timerlabel", widgets.timer);
	builder->get_widget("wpmlabel", widgets.wpm);
	builder->get_widget("wordslabel", widgets.wordNum);
	builder->get_widget("correctwordslabel", widgets.wordsCorrect);
	builder->get_widget("wrongwordslabel", widgets.wordsWrong);
	builder->get_widget("characterslabel", widgets.charNum);
	builder->get_widget("correctcharlabel", widgets.charsCorrect);
	builder->get_widget("wrongcharlabel", widgets.charsWrong);

	testBox->override_font(Pango::FontDescription("15"));
	widgets.entry->override_font(Pango::FontDescription("20"));
	widgets.timer->override_font(Pango::FontDescription("20"));
	widgets.textView->override_font(Pango::FontDescription("30"));

	Glib::RefPtr<Gtk::TextBuffer> textBuffer =
		Glib::RefPtr<Gtk::TextBuffer>::cast_static(builder->get_object("textbuffer"));
	Glib::RefPtr<Gtk::TextTagTable> textTags =
		Glib::RefPtr<Gtk::TextTagTable>::cast_static(builder->get_object("texttags"));

	Glib::RefPtr<Gtk::TextTag> currentTag =
		Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("currenttag"));
	Glib::RefPtr<Gtk::TextTag> currentErrorTag = 
		Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("currenterrortag"));
	Glib::RefPtr<Gtk::TextTag> errorTag =
		Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("errortag"));
	Glib::RefPtr<Gtk::TextTag> goodTag =
		Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("goodtag"));

	textTags->add(currentTag);
	textTags->add(currentErrorTag);
	textTags->add(errorTag);
	textTags->add(goodTag);

	Glib::RefPtr<Gtk::EntryBuffer> entryBuffer =
		Glib::RefPtr<Gtk::EntryBuffer>::cast_static(builder->get_object("typingbuffer"));

	//TypingTest test(widgets, 200, 2, 100, std::chrono::seconds(30),
	//		std::chrono::system_clock::now().time_since_epoch().count());
	
	test = new TypingTest(widgets, 200, 2, 100, std::chrono::seconds(60),
			std::chrono::system_clock::now().time_since_epoch().count());

	newTest->signal_clicked().connect(sigc::ptr_fun(&genNewTest));
	quit->signal_activate().connect(sigc::mem_fun(appWindow, &Gtk::ApplicationWindow::close));

	return app->run(*appWindow);
}
