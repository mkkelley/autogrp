#pragma once

#include <vector>
#include <string>

class Config;
std::vector<std::string> get_directory_contents(const std::string& path);
std::vector<std::string> download_missing_games(Config* config);
