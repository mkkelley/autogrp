//
// Created by Michael Kelley on 3/24/2018.
//

#pragma once

#include <string>
#include "work_queue.h"

typedef void CURL;
class INIReader;

struct JobInfo {
    BOT bot;
    std::string server_save_location;
    std::string filename;
    std::string client_save_location;
};

struct work_client {
    explicit work_client(INIReader*);
    ~work_client();

    std::optional<JobInfo> get_job();
    void submit_job(const JobInfo& job_info);

private:
    INIReader* config;
    std::string job_url;
    std::string submit_url;
    CURL* handle;
};


