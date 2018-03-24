#pragma once

#include <vector>
#include <string>

class INIReader;
std::vector<std::string> download_missing_games(INIReader* config);
