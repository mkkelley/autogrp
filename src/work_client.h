//
// Created by Michael Kelley on 3/24/2018.
//

#pragma once

#include <string>
#include "work_queue.h"

typedef void CURL;
class ClientConfig;

struct JobInfo {
    BOT bot;
    std::string server_save_location;
    std::string filename;
    std::string client_save_location;
};

struct work_client {
    explicit work_client(ClientConfig*);
    ~work_client();

    std::optional<JobInfo> get_job();
    void do_job(const JobInfo& job_info);
    void submit_job(const JobInfo& job_info);

private:
    ClientConfig* config;
    std::string job_url;
    std::string submit_url;
    CURL* handle;
};


