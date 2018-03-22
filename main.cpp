#include <iostream>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "windows.h"

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

void run_downloader(const boost::system::error_code& e) {
    if (!keep_running) return;

    std::cout << get_timestamp() << " Running downloader\n";
    boost::process::child downloader(boost::process::search_path("python.exe"),
                                     "C:/Users/purti/documents/go/download_ogs_games.py");
    downloader.wait();
    downloader.terminate();
}

int main() {
    SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(CtrlHandler), true);
    AmqpClient::Channel::ptr_t cxn = AmqpClient::Channel::Create("localhost");
    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::hours(1));
    t.async_wait(&run_downloader);
    boost::thread timer_thread(boost::bind(&boost::asio::io_service::run, &io));
    run_downloader(boost::system::error_code());

    while (keep_running) {
        std::string consumer_tag = cxn->BasicConsume("games", "");
        AmqpClient::Envelope::ptr_t envelope;
        cxn->BasicConsumeMessage(consumer_tag, envelope, 1000);
        if (!envelope) {
            cxn->BasicCancel(consumer_tag);
            continue;
        }

        cxn->BasicAck(envelope);
        std::string filename = envelope->Message()->Body();
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