//
// Created by Michael Kelley on 3/24/2018.
//

#include "work_client.h"

#include <fstream>
#include <curl/curl.h>
#include <INIReader.h>
#include "downloader.h"


size_t parse_job_info(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string header(ptr, size * nmemb);
    auto job_info = reinterpret_cast<JobInfo*>(userdata);
    if (header.find("analysis_bot") != std::string::npos) {
        auto bot_start = header.find(':') + 2;
        std::string bot_name = header.substr(bot_start);
        job_info->bot = bot_from_string(bot_name);
    } else if (header.find("server_save_location") != std::string::npos) {
        auto ssl_start = header.find(':') + 2;
        job_info->server_save_location = header.substr(ssl_start);
        auto filename_start = job_info->server_save_location.find_last_of('/') + 1;
        job_info->filename = job_info->server_save_location.substr(filename_start);
    }
    return size * nmemb;
}

size_t write_rsgf(char* buffer, size_t size, size_t nitems, void* userdata) {
    auto* rsgf = reinterpret_cast<std::ifstream*>(userdata);
    rsgf->read(buffer, size * nitems);
    return static_cast<size_t>(rsgf->gcount());
}

work_client::work_client(INIReader* config) :
        config(config),
        handle(curl_easy_init())
{
    job_url.append("http://")
            .append(config->Get("server", "address", "localhost"))
            .append(":")
            .append(config->Get("server", "port", "18185"));
    submit_url = job_url;
    job_url += "/job";
    submit_url += "/submit_job";
}

work_client::~work_client() {
    curl_easy_cleanup(handle);
}


std::optional<JobInfo> work_client::get_job() {
    std::string sgf;
    JobInfo job_info;
    curl_easy_reset(handle);
    curl_easy_setopt(handle, CURLOPT_URL, job_url.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, partial_save_result);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &sgf);
    curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, parse_job_info);
    curl_easy_setopt(handle, CURLOPT_HEADERDATA, &job_info);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_perform(handle);
    long response_code;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response_code);
    if (response_code != 200 || // server returns 204 when there is no job to be found
            job_info.filename.empty() || job_info.server_save_location.empty()) {
        return std::nullopt;
    }
    job_info.client_save_location = config->Get("client", "temp_sgf_path", ".") + "/" + job_info.filename;
    std::ofstream sgf_file(job_info.client_save_location, std::ios::binary);
    sgf_file.write(sgf.c_str(), sgf.size());
    sgf_file.close();
    return std::make_optional(job_info);
}

void work_client::submit_job(const JobInfo& job_info) {
    std::string save_location_header = "server_save_location: " + job_info.server_save_location;
    curl_slist* headers = NULL;
    headers = curl_slist_append(headers, save_location_header.c_str());
    std::ifstream rsgf_file(job_info.client_save_location, std::ios::binary);
    std::string rsgf;
    rsgf_file.seekg(0, std::ios::end);
    rsgf.reserve(static_cast<unsigned long long int>(rsgf_file.tellg()));
    rsgf_file.seekg(0, std::ios::beg);
    rsgf.assign((std::istreambuf_iterator<char>(rsgf_file)), std::istreambuf_iterator<char>());

    curl_easy_reset(handle);
    curl_easy_setopt(handle, CURLOPT_URL, submit_url.c_str());
    curl_easy_setopt(handle, CURLOPT_POST, 1);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, rsgf.c_str());
//    curl_easy_setopt(handle, CURLOPT_READFUNCTION, write_rsgf);
//    curl_easy_setopt(handle, CURLOPT_READDATA, &rsgf);
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_perform(handle);
    curl_slist_free_all(headers);

}
