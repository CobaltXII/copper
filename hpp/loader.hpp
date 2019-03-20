#pragma once

std::map<std::string, std::map<std::string, std::string>> ini_file;

int inii(std::string section, std::string name)
{
	return std::stoi(ini_file.at(section).at(name));
}

float inif(std::string section, std::string name)
{
	return std::stof(ini_file.at(section).at(name));
}

std::string inis(std::string section, std::string name)
{
	return ini_file.at(section).at(name);
}

int ini_parser(void* user, const char* section, const char* name, const char* value)
{
	ini_file[std::string(section)][std::string(name)] = value;

	return 1;
};