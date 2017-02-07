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

#include "config.h"

#include <sys/stat.h>

#include <err.h>

#include <iostream>
#include <cstdlib>
#include <glibmm.h>
#include <glibmm/keyfile.h>

namespace typingtest {

Config::Config()
{
	setPaths();
}

void Config::loadConfig()
{
	Glib::KeyFile settingsFile;
	try {
		if (settingsFile.load_from_file(configDir + "typingtest.conf")) {
			startWords = settingsFile.get_integer("Test Setup", "start_words");
			wordWrongWeight = settingsFile.get_double("Trouble Words",
					"word_wrong_weight");
			minZScore = settingsFile.get_double("Trouble Words", "min_zscore");
			maxZScore = settingsFile.get_double("Trouble Words", "max_zscore");
			startTroubleScore = settingsFile.get_integer("Trouble Words",
					"start_trouble_score");
			troubleDec = settingsFile.get_integer("Trouble Words", "trouble_dec");
			troubleInc = settingsFile.get_integer("Trouble Words", "trouble_inc");
		} else {
			saveConfig();
		}
	} catch (Glib::FileError) {
		saveConfig();
	} catch (Glib::KeyFileError) {
		saveConfig();
	}
}

void Config::saveConfig()
{
	Glib::KeyFile settingsFile;
	settingsFile.set_integer("Test Setup", "start_words", startWords);
	settingsFile.set_double("Trouble Words", "word_wrong_weight",
			wordWrongWeight);
	settingsFile.set_double("Trouble Words", "min_zscore", minZScore);
	settingsFile.set_double("Trouble Words", "max_zscore", maxZScore);
	settingsFile.set_integer("Trouble Words", "start_trouble_score",
			startTroubleScore);
	settingsFile.set_integer("Trouble Words", "trouble_dec", troubleDec);
	settingsFile.set_integer("Trouble Words", "trouble_inc", troubleInc);
	settingsFile.save_to_file(configDir + "typingtest.conf");
}

void Config::setPaths()
{
	configDir = Glib::get_user_config_dir() + "/typingtest/";
	mkdir(configDir.c_str(), 0755);

	dataDir = Glib::get_user_data_dir() + "/typingtest/";
	mkdir(dataDir.c_str(), 0755);
}
} // namespace typingtest
