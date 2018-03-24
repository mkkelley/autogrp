
#include "work_queue.h"

#include <boost/thread.hpp>

std::vector<std::pair<std::string, BOT>> work_queue;
boost::mutex work_queue_lock;

void add_to_queue(const std::string& filename, BOT bot) {
    work_queue_lock.lock();
    work_queue.emplace_back(filename, bot);
    work_queue_lock.unlock();
}

void add_to_queue(const std::string& filename) {
    add_to_queue(filename, bot_from_string(""));
}

void add_to_queue(const std::vector<std::pair<std::string, BOT>>& jobs) {
    work_queue_lock.lock();
    work_queue.reserve(jobs.size() + work_queue.size());
    work_queue.insert(work_queue.end(), jobs.begin(), jobs.end());
    work_queue_lock.unlock();
}

void add_to_queue(const std::vector<std::string>& filenames) {
    std::vector<std::pair<std::string, BOT>> jobs;
    jobs.reserve(filenames.size());
    for (const auto& filename : filenames) {
        jobs.emplace_back(filename, bot_from_string(""));
    }
    add_to_queue(jobs);
}

std::optional<std::pair<std::string, BOT>> get_job() {
    if (work_queue.empty()) return std::nullopt;

    work_queue_lock.lock();
    auto job = work_queue[0];
    work_queue.erase(work_queue.begin());
    work_queue_lock.unlock();

    return std::make_optional(job);
}

BOT bot_from_string(const std::string& bot_name) {
    if (bot_name == "leela_zero") {
        return BOT::LEELA_ZERO;
    } else {
        return BOT::LEELA;
    }
}
