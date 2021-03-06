
#include <iostream>
#include <boost/system/error_code.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/thread/thread.hpp>
#include "work_client.h"
#include "INIReader.h"
#include "config.h"

bool keep_running = true;

void ctrl_handler(const boost::system::error_code& e, int signal_number) {
    if (e) return;
    if (signal_number == SIGINT || signal_number == SIGTERM) {
        std::cout << "Stopping. Please wait up to one second or until the current job is finished.\n";
        keep_running = false;
    }
}

int main(int argc, char** argv) {
    std::string client_config_path = "client_config.ini";
    if (argc > 1) {
        client_config_path = argv[1];
    }
    INIReader client_config_reader(client_config_path);
    ClientConfig client_config(&client_config_reader);
    boost::asio::io_service io;
    boost::asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(ctrl_handler);
    boost::thread io_thread(boost::bind(&boost::asio::io_service::run, &io));
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

    io_thread.join();

    return 0;
}
