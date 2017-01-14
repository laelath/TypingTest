#include "config.h"

#include <iostream>
#include <fstream>

Config config;

void loadConfig()
{
	std::ifstream file("config.cfg");
	if (!file.is_open()) {
		std::exit(1);
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
	std::ofstream file(".config.cfg.swp", std::ios::trunc);
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

	std::remove("config.cfg");
	std::rename(".config.cfg.swp", "config.cfg");
}
