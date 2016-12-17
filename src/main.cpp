#include <chrono>
#include <iostream>

#include <pangomm/fontdescription.h>

#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/fontchooser.h>
#include <gtkmm/image.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#include "typing_test.h"

//Main window widgets
Gtk::ApplicationWindow *appWindow;
Gtk::Button *newTest;
Gtk::ImageMenuItem *settingsItem;
Gtk::ImageMenuItem *fontItem;
Gtk::ImageMenuItem *quitItem;
Gtk::Box *testBox;

Glib::RefPtr<Gtk::TextBuffer> textBuffer;
Glib::RefPtr<Gtk::TextTagTable> textTags;
Glib::RefPtr<Gtk::TextTag> currentTag;
Glib::RefPtr<Gtk::TextTag> currentErrorTag;
Glib::RefPtr<Gtk::TextTag> errorTag;
Glib::RefPtr<Gtk::TextTag> goodTag;
Glib::RefPtr<Gtk::TextTag> uglyHackTag;

TestWidgets widgets;
TypingTest *test;

//Settings window widgets
Gtk::Dialog *settingsDialog;
Gtk::Button *cancel;
Gtk::Button *apply;
Gtk::RadioButton *basicOption;
Gtk::RadioButton *advancedOption;
Gtk::RadioButton *enduranceOption;
Gtk::RadioButton *customOption;
Gtk::SpinButton *topWords;
Gtk::SpinButton *minWordLength;
Gtk::SpinButton *maxWordLength;
Gtk::SpinButton *testLength;
Gtk::Entry *seedEntry;

Gtk::Button *randomizeSeed;

TestSettings settings = basic_test;

//Font chooser
Gtk::Dialog *fontDialog;
Gtk::FontChooser *fontChooser;
Gtk::Button *fontCancel;
Gtk::Button *fontApply;

std::string currFont = "Sans 25";

void genNewTest()
{
	delete test;
	test = new TypingTest(widgets, settings);
}

void updateSettings(TestType type)
{
	if (type == CUSTOM) {
		topWords->set_sensitive(true);
		minWordLength->set_sensitive(true);
		maxWordLength->set_sensitive(true);
		testLength->set_sensitive(true);
		seedEntry->set_sensitive(true);
		randomizeSeed->set_sensitive(true);

		settings.type = CUSTOM;
	} else {
		topWords->set_sensitive(false);
		minWordLength->set_sensitive(false);
		maxWordLength->set_sensitive(false);
		testLength->set_sensitive(false);
		seedEntry->set_sensitive(false);
		randomizeSeed->set_sensitive(false);

		settings = getTestTypeSettings(type);
	}

	topWords->set_value(settings.topWords);
	minWordLength->set_value(settings.minLength);
	maxWordLength->set_value(settings.maxLength);
	testLength->set_value(settings.seconds.count());
	seedEntry->set_text(std::to_string(settings.seed));
}

void randomSeed()
{
	seedEntry->set_text(std::to_string(
				std::chrono::system_clock::now().time_since_epoch().count() % UINT32_MAX));
}

void openSettings()
{
	if (settings.type == BASIC) {
		basicOption->clicked();
	} else if (settings.type == ADVANCED) {
		advancedOption->clicked();
	} else if (settings.type == ENDURANCE) {
		enduranceOption->clicked();
	} else if (settings.type == CUSTOM) {
		customOption->clicked();
	}

	//updateSettingsWidgets();
	settingsDialog->present();
}

void applySettings()
{
	settings.topWords = topWords->get_value_as_int();
	settings.minLength = minWordLength->get_value_as_int();
	settings.maxLength = maxWordLength->get_value_as_int();
	settings.seconds = std::chrono::seconds(testLength->get_value_as_int());
	settings.seed = std::stoul(seedEntry->get_text());
	
	genNewTest();
	settingsDialog->close();
}

void openFont()
{
	fontChooser->set_font(currFont);
	fontDialog->present();
}

void applyFont(std::string font)
{
	currFont = font;
	widgets.textView->override_font(Pango::FontDescription(font));
	fontDialog->close();
}

void applyFont()
{
	applyFont(fontChooser->get_font());
}

int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "us.laelath.typingtest");

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
	builder->add_from_file("ui/typingui.glade");

	//Prepare main window
	builder->get_widget("typingtest", appWindow);
	builder->get_widget("newtestbutton", newTest);
	builder->get_widget("settings", settingsItem);
	builder->get_widget("font", fontItem);
	builder->get_widget("quit", quitItem);
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
	widgets.textView->override_font(Pango::FontDescription(currFont));

	textBuffer = Glib::RefPtr<Gtk::TextBuffer>::cast_static(builder->get_object("textbuffer"));
	textTags = Glib::RefPtr<Gtk::TextTagTable>::cast_static(builder->get_object("texttags"));
	currentTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("currenttag"));
	currentErrorTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("currenterrortag"));
	errorTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("errortag"));
	goodTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("goodtag"));
	uglyHackTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("uglyhacktag"));

	textTags->add(currentTag);
	textTags->add(currentErrorTag);
	textTags->add(errorTag);
	textTags->add(goodTag);
	textTags->add(uglyHackTag);

	//Prepare settings window
	builder->get_widget("settingsdialog", settingsDialog);
	builder->get_widget("cancelbutton", cancel);
	builder->get_widget("applybutton", apply);
	builder->get_widget("basictest", basicOption);
	builder->get_widget("advancedtest", advancedOption);
	builder->get_widget("endurancetest", enduranceOption);
	builder->get_widget("customtest", customOption);
	builder->get_widget("topwordsentry", topWords);
	builder->get_widget("minlengthentry", minWordLength);
	builder->get_widget("maxlengthentry", maxWordLength);
	builder->get_widget("testlengthentry", testLength);
	builder->get_widget("seedentry", seedEntry);
	builder->get_widget("randomizeseed", randomizeSeed);

	//Font settings window
	builder->get_widget("fontdialog", fontDialog);
	builder->get_widget("fontchooser", fontChooser);
	builder->get_widget("fontcancel", fontCancel);
	builder->get_widget("fontapply", fontApply);

	//Connect signals
	newTest->signal_clicked().connect(sigc::ptr_fun(&genNewTest));
	settingsItem->signal_activate().connect(sigc::ptr_fun(&openSettings));
	fontItem->signal_activate().connect(sigc::ptr_fun(&openFont));
	quitItem->signal_activate().connect(sigc::mem_fun(appWindow, &Gtk::ApplicationWindow::close));

	basicOption->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&updateSettings), BASIC));
	advancedOption->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&updateSettings), ADVANCED));
	enduranceOption->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&updateSettings), ENDURANCE));
	customOption->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&updateSettings), CUSTOM));
	randomizeSeed->signal_clicked().connect(sigc::ptr_fun(&randomSeed));
	apply->signal_clicked().connect(sigc::ptr_fun(&applySettings));
	cancel->signal_clicked().connect(sigc::mem_fun(settingsDialog, &Gtk::Dialog::close));

	fontApply->signal_clicked().connect(sigc::ptr_fun0(&applyFont));
	fontChooser->signal_font_activated().connect(sigc::ptr_fun1(&applyFont));
	fontCancel->signal_clicked().connect(sigc::mem_fun(fontDialog, &Gtk::Dialog::close));

	//Create test
	test = new TypingTest(widgets);

	return app->run(*appWindow);
}
