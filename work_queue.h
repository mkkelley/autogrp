#pragma once

#include <string>
#include <vector>
#include <optional>

enum class BOT { LEELA, LEELA_ZERO };
BOT bot_from_string(const std::string& bot_name);

void add_to_queue(const std::string& filename);
void add_to_queue(const std::string& filename, BOT bot);
void add_to_queue(const std::vector<std::string>&);
void add_to_queue(const std::vector<std::pair<std::string, BOT>>&);

std::optional<std::pair<std::string, BOT>> get_job();
