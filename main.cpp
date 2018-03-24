#include <iostream>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "windows.h"
#include "downloader.h"
#include "INIReader.h"
#include "work_queue.h"
#include "work_client.h"
#include "logutils.h"
#include "work_server.h"

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

void run_downloader(const boost::system::error_code& e, INIReader* config) {
    if (!keep_running) return;
    std::cout << get_timestamp() << " Running downloader\n";
    std::vector<std::string> new_games = download_missing_games(config);
    add_to_queue(new_games);
}

int main() {
    SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(CtrlHandler), true);
    INIReader reader("config.ini");
    INIReader client_config("client_config.ini");
    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::hours(1));
    t.async_wait(boost::bind(run_downloader, boost::asio::placeholders::error, &reader));
    boost::thread timer_thread(boost::bind(&boost::asio::io_service::run, &io));
    run_downloader(boost::system::error_code(), &reader);
    start_server(&reader);
    work_client c(&client_config);

    while (keep_running) {
        auto opt_job = c.get_job();
        if (!opt_job.has_value()) {
            Sleep(1000);
            continue;
        }
        auto job = opt_job.value();

        c.do_job(job);
    }

    t.cancel();
    timer_thread.join();

    return 0;
}