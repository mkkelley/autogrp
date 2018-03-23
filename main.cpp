#include <iostream>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "windows.h"
#include "downloader.h"
#include "INIReader.h"

bool keep_running = true;
std::vector<std::string> work_queue;
boost::mutex work_queue_mutex;

bool CtrlHandler(DWORD fdwCtrlType) {
    if (fdwCtrlType == CTRL_C_EVENT) {
        keep_running = false;
        std::cout << "Stopping. Please wait up to one second or until the current job is finished.\n";
        return true;
    } else {
        return false;
    }
}

std::string get_timestamp() {
    char timestamp[20];
    time_t now = time(nullptr);
    strftime(timestamp + 1, 19, "%d/%m/%y %H:%M:%S", localtime(&now));
    timestamp[0] = '[';
    timestamp[sizeof(timestamp)/sizeof(char) - 1] = '\0';
    timestamp[sizeof(timestamp)/sizeof(char) - 2] = ']';
    return std::string(timestamp);
}

void run_downloader(const boost::system::error_code& e, const std::string& player_id, const std::string& games_dir) {
    if (!keep_running) return;
    std::cout << get_timestamp() << " Running downloader\n";
    std::vector<std::string> new_games = download_missing_games(player_id, games_dir);
    work_queue_mutex.lock();
    work_queue.reserve(work_queue.size() + new_games.size());
    work_queue.insert(work_queue.end(), new_games.begin(), new_games.end());
    work_queue_mutex.unlock();
}

int main() {
    SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(CtrlHandler), true);
    INIReader reader("config.ini");
    std::string player_id = reader.Get("core", "ogs_id", "");
    std::string game_dir = reader.Get("core", "games_dir", "");
    if (player_id.empty() || game_dir.empty()) return 1;
    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::hours(1));
    t.async_wait(boost::bind(run_downloader, boost::asio::placeholders::error, player_id, game_dir));
    boost::thread timer_thread(boost::bind(&boost::asio::io_service::run, &io));
    run_downloader(boost::system::error_code(), player_id, game_dir);

    while (keep_running) {
        work_queue_mutex.lock();
        if (work_queue.empty()) {
            work_queue_mutex.unlock();
            Sleep(1000);
            continue;
        }

        std::string filename = work_queue[0];
        work_queue.erase(work_queue.begin());
        work_queue_mutex.unlock();

        std::cout << get_timestamp() << " Starting to process: " << filename;
        boost::process::child analysis("C:/Anaconda3/envs/py27/python.exe",
                                       "C:/Users/purti/documents/go/goreviewpartner-master/leela_analysis.py",
                                       "--no-gui",
                                       filename);
        analysis.wait();
        analysis.terminate();
    }

    t.cancel();
    timer_thread.join();

    return 0;
}