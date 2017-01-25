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

#include <chrono>
#include <fstream>
#include <iostream>

#include <pangomm/fontdescription.h>

#include <gtkmm/aboutdialog.h>
#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/fontchooserdialog.h>
#include <gtkmm/image.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/liststore.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#include "config.h"
#include "typing_test.h"

//Main window widgets
Gtk::ApplicationWindow *appWindow;
Gtk::Button *newTest;
Gtk::ImageMenuItem *settingsItem;
Gtk::ImageMenuItem *fontItem;
Gtk::ImageMenuItem *advItem;
Gtk::ImageMenuItem *quitItem;
Gtk::ImageMenuItem *troubleItem;
Gtk::ImageMenuItem *aboutItem;
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
Gtk::ComboBoxText *testTypeBox;
Gtk::SpinButton *topWords;
Gtk::SpinButton *minWordLength;
Gtk::SpinButton *maxWordLength;
Gtk::SpinButton *testLength;
Gtk::Entry *seedEntry;
Gtk::Button *randomizeSeed;
Gtk::SpinButton *personalFrequency;

// Font chooser
Gtk::FontChooserDialog *fontChooser;

// Trouble words window
Gtk::Dialog *troubleDialog;
Gtk::TreeView *troubleList;
Gtk::Button *troubleClose;

Glib::RefPtr<Gtk::ListStore> troubleListStore;

Gtk::TreeModelColumn<std::string> strCol;
Gtk::TreeModelColumn<unsigned int> valCol;

//Test settings
TestSettings settings = basic_test;
std::string currFont = "Sans 25";

//Advanced settings widgets
Gtk::Dialog *advSettingsDialog;
Gtk::SpinButton *startWords;
Gtk::SpinButton *minZScore;
Gtk::SpinButton *maxZScore;
Gtk::SpinButton *startTroubleScore;
Gtk::SpinButton *troubleDec;
Gtk::SpinButton *troubleInc;
Gtk::SpinButton *wordWrongMult;
Gtk::Button *restoreDefaultAdv;
Gtk::Button *cancelAdv;
Gtk::Button *applyAdv;

//About dialog
Gtk::AboutDialog *aboutDialog;

void genNewTest()
{
	delete test;
	test = new TypingTest(appWindow, widgets, settings);
}

void updateSettings()
{
	TestType type = getTypeFromNumber(testTypeBox->get_active_row_number());
	switch (type) {
		case CUSTOM :
			topWords->set_sensitive(true);
			minWordLength->set_sensitive(true);
			maxWordLength->set_sensitive(true);
			testLength->set_sensitive(true);
			seedEntry->set_sensitive(true);
			randomizeSeed->set_sensitive(true);
			personalFrequency->set_sensitive(true);
			settings.type = CUSTOM;
			break;
		default :
			topWords->set_sensitive(false);
			minWordLength->set_sensitive(false);
			maxWordLength->set_sensitive(false);
			testLength->set_sensitive(false);
			seedEntry->set_sensitive(false);
			randomizeSeed->set_sensitive(false);
			personalFrequency->set_sensitive(false);
			settings = getTestTypeSettings(type);
			break;
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
	testTypeBox->set_active(getTypeNumber(settings.type));
	updateSettings();
	int response = settingsDialog->run();
	if (response == Gtk::RESPONSE_APPLY) {
		settings.topWords = topWords->get_value_as_int();
		settings.minLength = minWordLength->get_value_as_int();
		settings.maxLength = maxWordLength->get_value_as_int();
		settings.seconds = std::chrono::seconds(testLength->get_value_as_int());
		settings.seed = std::stoul(seedEntry->get_text());
		settings.personalFrequency = personalFrequency->get_value();

		genNewTest();
	}
	settingsDialog->close();
}

void openFont()
{
	fontChooser->set_font(currFont);
	int response = fontChooser->run();
	if (response == Gtk::RESPONSE_OK) {
		widgets.textView->override_font(Pango::FontDescription(fontChooser->get_font()));
	}
	fontChooser->close();
}

void updateAdvSettings()
{
	startWords->set_value(config.startWords);
	minZScore->set_value(config.minZScore);
	maxZScore->set_value(config.maxZScore);
	startTroubleScore->set_value(config.startTroubleScore);
	troubleDec->set_value(config.troubleDec);
	troubleInc->set_value(config.troubleInc);
	wordWrongMult->set_value(config.wordWrongWeight);
}

void applyDefaultSettings()
{
	Config newConfig;
	config = newConfig;

	updateAdvSettings();
}

void openAdvSettings()
{
	updateAdvSettings();

	int response = advSettingsDialog->run();
	if (response == Gtk::RESPONSE_APPLY) {
		config.startWords = startWords->get_value_as_int();
		config.minZScore = minZScore->get_value();
		config.maxZScore = maxZScore->get_value();
		config.startTroubleScore = startTroubleScore->get_value_as_int();
		config.troubleDec = troubleDec->get_value_as_int();
		config.troubleInc = troubleInc->get_value_as_int();
		config.wordWrongWeight = wordWrongMult->get_value();
		saveConfig();
		genNewTest();
	}

	advSettingsDialog->close();
}

void openTroubleWords()
{
	std::ifstream trWords(data_dir + "troublewords.txt");

	troubleListStore->clear();

	std::string line;
	while (std::getline(trWords, line)) {
		std::string word = line.substr(0, line.find(","));
		unsigned int val = std::stoi(line.substr(line.find(",") + 1));
		Gtk::ListStore::Row row = *(troubleListStore->append());
		row[strCol] = word;
		row[valCol] = val;
	}

	troubleDialog->run();
	troubleDialog->close();
}

void openAbout()
{
	aboutDialog->run();
	aboutDialog->close();
}

int main(int argc, char *argv[])
{
	getPaths();
	loadConfig();

	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "us.laelath.typingtest");
	Glib::RefPtr<Gtk::Builder> builder =
		Gtk::Builder::create_from_resource("/us/laelath/typingtest/ui/typingui.glade");

	//Prepare main window
	builder->get_widget("typingtest", appWindow);
	builder->get_widget("newtestbutton", newTest);
	builder->get_widget("settings", settingsItem);
	builder->get_widget("font", fontItem);
	builder->get_widget("advanced", advItem);
	builder->get_widget("quit", quitItem);
	builder->get_widget("viewtroublewords", troubleItem);
	builder->get_widget("about", aboutItem);
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
	builder->get_widget("troublewordslabel", widgets.troubleWords);

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
	builder->get_widget("testtypebox", testTypeBox);
	builder->get_widget("topwordsentry", topWords);
	builder->get_widget("minlengthentry", minWordLength);
	builder->get_widget("maxlengthentry", maxWordLength);
	builder->get_widget("testlengthentry", testLength);
	builder->get_widget("seedentry", seedEntry);
	builder->get_widget("randomizeseed", randomizeSeed);
	builder->get_widget("personalratioentry", personalFrequency);

	//Font settings window
	fontChooser = new Gtk::FontChooserDialog("Select a font", *appWindow);

	//Advanced settings window
	builder->get_widget("advancedsettingsdialog", advSettingsDialog);
	builder->get_widget("startwords", startWords);
	builder->get_widget("minzscore", minZScore);
	builder->get_widget("maxzscore", maxZScore);
	builder->get_widget("starttrouble", startTroubleScore);
	builder->get_widget("troubledec", troubleDec);
	builder->get_widget("troubleinc", troubleInc);
	builder->get_widget("wordwrongmult", wordWrongMult);
	builder->get_widget("restoredefaultadv", restoreDefaultAdv);
	builder->get_widget("canceladv", cancelAdv);
	builder->get_widget("applyadv", applyAdv);

	//Trouble words viewer
	builder->get_widget("troubledialog", troubleDialog);
	builder->get_widget("troublelist", troubleList);
	builder->get_widget("troubleclosebutton", troubleClose);

	Gtk::TreeModelColumnRecord cols;
	cols.add(strCol);
	cols.add(valCol);

	troubleListStore = Gtk::ListStore::create(cols);
	troubleList->set_model(troubleListStore);

	troubleList->append_column("Trouble Word", strCol);
	troubleList->append_column("Weight", valCol);

	troubleListStore->set_sort_column(valCol, Gtk::SortType::SORT_DESCENDING);

	//About dialog
	builder->get_widget("aboutdialog", aboutDialog);

	//Connect signals
	newTest->signal_clicked().connect(sigc::ptr_fun(&genNewTest));
	settingsItem->signal_activate().connect(sigc::ptr_fun(&openSettings));
	fontItem->signal_activate().connect(sigc::ptr_fun(&openFont));
	advItem->signal_activate().connect(sigc::ptr_fun(&openAdvSettings));
	quitItem->signal_activate().connect(sigc::mem_fun(appWindow, &Gtk::ApplicationWindow::close));
	troubleItem->signal_activate().connect(sigc::ptr_fun(&openTroubleWords));
	aboutItem->signal_activate().connect(sigc::ptr_fun(&openAbout));

	testTypeBox->signal_changed().connect(sigc::ptr_fun(&updateSettings));
	randomizeSeed->signal_clicked().connect(sigc::ptr_fun(&randomSeed));
	apply->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(settingsDialog, &Gtk::Dialog::response), Gtk::RESPONSE_APPLY));
	cancel->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(settingsDialog, &Gtk::Dialog::response), Gtk::RESPONSE_CANCEL));

	applyAdv->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(advSettingsDialog, &Gtk::Dialog::response), Gtk::RESPONSE_APPLY));
	cancelAdv->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(advSettingsDialog, &Gtk::Dialog::response), Gtk::RESPONSE_CANCEL));
	restoreDefaultAdv->signal_clicked().connect(sigc::ptr_fun(&applyDefaultSettings));

	troubleClose->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(troubleDialog, &Gtk::Dialog::response), Gtk::RESPONSE_CLOSE));

	//Create test
	test = new TypingTest(appWindow, widgets);

	return app->run(*appWindow);
}
