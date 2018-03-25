#include <iostream>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "downloader.h"
#include "INIReader.h"
#include "work_queue.h"
#include "work_client.h"
#include "logutils.h"
#include "work_server.h"

bool keep_running = true;

void ctrl_handler(const boost::system::error_code& e, int signal_number) {
    if (e) return;
    if (signal_number == SIGINT || signal_number == SIGTERM) {
        std::cout << "Stopping. Please wait up to one second or until the current job is finished.\n";
        keep_running = false;
    }
}

void run_downloader(const boost::system::error_code& e, boost::asio::deadline_timer* timer, INIReader* config) {
    if (!keep_running) return;
    std::cout << get_timestamp() << " Running downloader\n";
    std::vector<std::string> new_games = download_missing_games(config);
    std::vector<std::string> bot_strings;
    std::string botconfig = config->Get("core", "bots_to_use", "leela");
    boost::split(bot_strings, botconfig, boost::is_any_of(" \t"), boost::token_compress_on);
    std::vector<BOT> bots_to_use;
    for (const auto& bot_string : bot_strings) {
        bots_to_use.push_back(bot_from_string(bot_string));
    }
    auto last = std::unique(bots_to_use.begin(), bots_to_use.end());
    bots_to_use.erase(last, bots_to_use.end());

    //constructing jobs outside so that we can use add to queue collection version instead of calling lock games*bots times
    std::vector<std::pair<std::string, BOT>> jobs;
    for (const auto& game : new_games) {
        for (BOT bot : bots_to_use) {
            jobs.emplace_back(game, bot);
        }
    }
    add_to_queue(jobs);
    timer->expires_at(timer->expires_at() + boost::posix_time::hours(1));
    timer->async_wait(boost::bind(run_downloader, boost::asio::placeholders::error, timer, config));
}

int main() {
    INIReader reader("config.ini");
    INIReader client_config("client_config.ini");
    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
    t.async_wait(boost::bind(run_downloader, boost::asio::placeholders::error, &t, &reader));
    boost::asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(ctrl_handler);
    boost::thread timer_thread(boost::bind(&boost::asio::io_service::run, &io));
    auto server_thread = start_server(&reader);

    if (reader.GetBoolean("core", "run_local_worker", true)) {
        work_client c(&client_config);

        while (keep_running) {
            auto opt_job = c.get_job();
            if (!opt_job.has_value()) {
                boost::this_thread::sleep_for(boost::chrono::seconds(1));
                continue;
            }
            auto job = opt_job.value();

            c.do_job(job);
            c.submit_job(job);
        }
    } else {
        server_thread->join();
    }

    t.cancel();
    timer_thread.join();

    return 0;
}