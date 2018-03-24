//
// Created by Michael Kelley on 3/23/2018.
//

#include "work_server.h"

#include <INIReader.h>
#define RESTINIO_USE_BOOST_ASIO shared
#include <restinio/all.hpp>
#include <fstream>

#include "work_queue.h"

template <typename ServerTraits>
void start_server_impl(INIReader* config) {
    const static int pool_size = 1;
    restinio::run(
            restinio::on_thread_pool<ServerTraits>(pool_size)
                    .port(config->GetInteger("server", "port", 18185))
                    .address(config->Get("server", "address", "localhost"))
                    .concurrent_accepts_count(pool_size)
                    .request_handler(
                            [&](auto request) {
                                if (request->header().method() == restinio::http_method_get() &&
                                    request->header().request_target() == "/job") {
                                    auto job_opt = get_job();
                                    if (!job_opt.has_value()) {
                                        return request->create_response(204)
                                                .append_header(restinio::http_field::server, "AutoGRP Job Server")
                                                .append_header_date_field()
                                                .done();
                                    } else {
                                        auto job = job_opt.value();
                                        return request->create_response()
                                                .append_header(restinio::http_field::server, "AutoGRP Job Server")
                                                .append_header_date_field()
                                                .set_body(job.first + " " + string_from_bot(job.second))
                                                .done();
                                    }
                                }
                                if (request->header().method() == restinio::http_method_post() &&
                                    request->header().request_target() == "/submit_job" ) {
                                    if (!request->header().has_field("local_save_location")) {
                                        return restinio::request_rejected();
                                    }
                                    std::string save_path = request->header().get_field("local_save_location");
                                    if (save_path.find(".rsgf") == std::string::npos) {
                                        return restinio::request_rejected();
                                    }
                                    std::ofstream outfile(save_path, std::ios::binary);
                                    auto& body = request->body();
                                    outfile.write(body.c_str(), body.size());
                                    outfile.close();
                                    return request->create_response()
                                            .append_header(restinio::http_field::server, "AutoGRP Job Server")
                                            .append_header_date_field()
                                            .done();
                                }
                                return restinio::request_rejected();
                            }
                    )
    );
}

void start_server(INIReader* config) {
    start_server_impl<restinio::default_traits_t>(config);
}


