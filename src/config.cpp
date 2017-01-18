#include "config.h"

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <sys/stat.h>

Config config;

std::string config_dir;
std::string data_dir;

void loadConfig()
{
	std::ifstream file(config_dir + "config.cfg");
	if (!file.is_open()) {
		saveConfig();
		return;
	}

	std::string line;
	while(std::getline(file, line)) {
		std::string var = line.substr(0, line.find("="));
		std::string param = line.substr(line.find("=") + 1);
		if (var == "start_words") {
			config.startWords = std::stoi(param);
		} else if (var == "min_zscore") {
			config.minZScore = std::stod(param);
		} else if (var == "max_zscore") {
			config.maxZScore = std::stod(param);
		} else if (var == "start_trouble_words") {
			config.startTroubleScore = std::stoi(param);
		} else if (var == "trouble_dec") {
			config.troubleDec = std::stoi(param);
		} else if (var == "trouble_inc") {
			config.troubleInc = std::stoi(param);
		} else if (var == "word_wrong_mult") {
			config.wordWrongWeight = std::stod(param);
		}
	}
}

void saveConfig()
{
	std::ofstream file(config_dir + ".config.cfg.swp", std::ios::trunc);
	if (!file.is_open()) {
		std::exit(1);
	}

	file << "start_words=" << config.startWords << "\n";
	file << "min_zscore=" << config.minZScore << "\n";
	file << "max_zscore=" << config.maxZScore << "\n";
	file << "start_trouble_words=" << config.startTroubleScore << "\n";
	file << "trouble_dec=" << config.troubleDec << "\n";
	file << "trouble_inc=" << config.troubleInc << "\n";
	file << "word_wrong_mult=" << config.wordWrongWeight << "\n";

	std::remove((config_dir + "config.cfg").c_str());
	std::rename((config_dir + ".config.cfg.swp").c_str(), (config_dir + "config.cfg").c_str());
}

void getPaths()
{
	std::string home;
	if (char *val = std::getenv("HOME")) {
		home = val;
	} else {
		std::cout << "HOME variable not found" << std::endl;
		std::exit(1);
	}

	if (char *val = std::getenv("XDG_CONFIG_HOME")) {
		config_dir = val;
	} else {
		config_dir = home + "/.config/typingtest/";
	}
	mkdir(config_dir.c_str(), 0755);

	if (char *val = std::getenv("XDG_DATA_HOME")) {
		data_dir = val;
	} else {
		data_dir = home + "/.local/share/typingtest/";
	}
	mkdir(data_dir.c_str(), 0755);
}
