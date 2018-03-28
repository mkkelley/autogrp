#pragma once

#include <string>

class INIReader;

struct Config {
    Config(INIReader* inireader);

    //core
    std::string ogs_id;
    std::string games_dir;
    int start_page;
    std::string bots_to_use;
    bool run_local_worker;

    //server
    int port;
    std::string address;
};

struct ClientConfig {
    ClientConfig(INIReader* inireader);

    //server
    int port;
    std::string address;

    //client
    std::string temp_sgf_path;
    std::string python_path;
    std::string leela_path;
    std::string leela_zero_path;

};
