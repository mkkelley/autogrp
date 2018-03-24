#pragma once

#include <vector>
#include <string>

class INIReader;
std::vector<std::string> download_missing_games(INIReader* config);
size_t partial_save_result(char* ptr, size_t size, size_t nmemb, void* userdata);
