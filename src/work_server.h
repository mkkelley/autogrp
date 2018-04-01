//
// Created by Michael Kelley on 3/23/2018.
//

#pragma once

#include <memory>
#include <boost/thread.hpp>

#include "work_queue.h"

class Config;

class WorkServer {
public:
    WorkServer(Config* config);
    virtual ~WorkServer() = default;
    
    std::unique_ptr<boost::thread> start();

protected:
    /**
     * Subclasses can override this function to do something when a job is served
     */
    virtual void job_served(const std::string&, BOT) {};

    /**
    * Subclasses can override this function to do something when a job is submitted
    */
    virtual void job_submitted(const std::string&, BOT) {};

private:
    template <typename T> void start_server_impl();
    void start_server_impl_c();

    Config* config;
};

