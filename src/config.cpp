#include "config.h"

#include <cstdlib>
#include "INIReader.h"

#define MISSING_GAMES_DIR 1
#define MISSING_TEMP_SGF_PATH 2
#define MISSING_PYTHON_PATH 3

Config::Config(INIReader* inireader) :
    ogs_id(inireader->Get("core", "ogs_id", "")),
    games_dir(inireader->Get("core", "games_dir", "")),
    start_page(inireader->GetInteger("core", "start_page", 1)),
    bots_to_use(inireader->Get("core", "bots_to_use", "leela")),
    run_local_worker(inireader->GetBoolean("core", "run_local_worker", true)),
    port(inireader->GetInteger("server", "port", 18185)),
    address(inireader->Get("server", "address", "localhost"))
{
    if (games_dir.empty()) {
        std::exit(MISSING_GAMES_DIR);
    }

}

ClientConfig::ClientConfig(INIReader *inireader) :
    port(inireader->GetInteger("server", "port", 18185)),
    address(inireader->Get("server", "address", "localhost")),
    temp_sgf_path(inireader->Get("cleint", "temp_sgf_path", "")),
    python_path(inireader->Get("client", "python_path", "")),
    leela_path(inireader->Get("client", "leela_path", "")),
    leela_zero_path(inireader->Get("client", "leela_zero_path", ""))
{
    if (temp_sgf_path.empty()) {
        std::exit(MISSING_TEMP_SGF_PATH);
    } else if (python_path.empty()) {
        std::exit(MISSING_PYTHON_PATH);
    }

}
