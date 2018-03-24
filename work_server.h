//
// Created by Michael Kelley on 3/23/2018.
//

#pragma once

#include <memory>
#include <boost/thread.hpp>

class INIReader;
std::unique_ptr<boost::thread> start_server(INIReader*);

