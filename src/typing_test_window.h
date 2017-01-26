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

#include <random>

#include <gtkmm.h>

#include "word.h"

class TypingTestWindow : public Gtk::ApplicationWindow {
public:
	TypingTestWindow(BaseObjectType *cobject,
		const Glib::RefPtr<Gtk::Builder>& builder);

private:
// Main window widgets
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

Gtk::Window *parent;
Gtk::TextView *textView;
Gtk::Entry *entry;
Gtk::Label *timerLabel;
Gtk::Label *wpmLabel;
Gtk::Label *wordNumLabel;
Gtk::Label *wordsCorrectLabel;
Gtk::Label *wordsWrongLabel;
Gtk::Label *charNumLabel;
Gtk::Label *charsCorrectLabel;
Gtk::Label *charsWrongLabel;
Gtk::Label *troubleWordsLabel;

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

void disconnectSignals();
std::string genWord();
std::string getWords();
std::string getTime();
void textInsert(std::string text, int *pos);
void textDelete(int pos, int num);
bool updateTimer();
void calculateScore();

sigc::connection insertConnection;
sigc::connection backspConnection;
sigc::connection timerConnection;

std::minstd_rand rand;

std::vector<std::string> wordSelection;
std::vector<std::string> personalSelection;
std::vector<Word> words;

double personalFrequency;

std::chrono::seconds seconds;
std::chrono::seconds start;

int wordIndex = 0;
int wordCharIndex = 0;
bool testStarted = false;
bool testEnded = false;
};
