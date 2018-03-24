#include <iostream>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "windows.h"
#include "downloader.h"
#include "INIReader.h"
#include "work_queue.h"

bool keep_running = true;

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

void run_downloader(const boost::system::error_code& e, INIReader* config) {
    if (!keep_running) return;
    std::cout << get_timestamp() << " Running downloader\n";
    std::vector<std::string> new_games = download_missing_games(config);
    add_to_queue(new_games);
}

int main() {
    SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(CtrlHandler), true);
    INIReader reader("config.ini");
    std::string python_exe_path = reader.Get("core", "python_exe_path", "");
    std::string analysis_path = reader.Get("core", "grp_analyze_path_flags", "");
    if (python_exe_path.empty() || analysis_path.empty()) return 1;
    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::hours(1));
    t.async_wait(boost::bind(run_downloader, boost::asio::placeholders::error, &reader));
    boost::thread timer_thread(boost::bind(&boost::asio::io_service::run, &io));
    run_downloader(boost::system::error_code(), &reader);

    while (keep_running) {
        auto opt_job = get_job();
        if (!opt_job.has_value()) {
            Sleep(1000);
            continue;
        }
        auto job = opt_job.value();

        std::string filename = job.first;

        std::cout << get_timestamp() << " Starting to process: " << filename;
        boost::process::child analysis(python_exe_path,
                                       analysis_path,
                                       "--no-gui",
                                       filename);
        analysis.wait();
        analysis.terminate();
    }

    t.cancel();
    timer_thread.join();

    return 0;
}