#include "downloader.h"

#include <curl/curl.h>
#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <rapidjson/document.h>
#include <INIReader.h>

size_t partial_save_result(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* whole_result = reinterpret_cast<std::string*>(userdata);
    std::string partial(ptr, size * nmemb);
    *whole_result += partial;
    return size * nmemb;
}

std::vector<std::string> get_directory_contents(const std::string& path) {
    using namespace boost::filesystem;
    std::vector<std::string> out;
    if (!is_directory(path)) return out;

    for (const auto& entry : boost::make_iterator_range(directory_iterator(path))) {
        out.emplace_back(entry.path().filename().generic_string());
    }
    return out;
}

std::vector<std::string> download_missing_games(INIReader* config) {
    std::string player_id = config->Get("core", "ogs_id", "");
    std::string game_dir = config->Get("core", "games_dir", "");
    int start_page = config->GetInteger("core", "start_page", 1);
    if (player_id.empty() || game_dir.empty()) exit(1);
    std::vector<std::string> new_files;
    auto directory_contents = get_directory_contents(game_dir);
    directory_contents.erase(std::remove_if(directory_contents.begin(), directory_contents.end(),
                                            [](const std::string& filename) { return filename.find(".sgf") == std::string::npos;}),
                             directory_contents.end());
    std::transform(directory_contents.begin(), directory_contents.end(), directory_contents.begin(),
                   [](std::string filename) -> std::string { return filename.substr(0, filename.size() - 4);}
    );
    auto handle = curl_easy_init();
    std::string next_url = "https://online-go.com/api/v1/players/" + player_id + "/games?page=" + std::to_string(start_page);
    bool has_next = true;
    int current_page = start_page - 1;

    while (has_next) {
        std::cout << "Searching for new games in: " << current_page * 10 + 1 << " to " << (current_page + 1) * 10 << std::endl;
        std::string data;
        curl_easy_setopt(handle, CURLOPT_URL, next_url.c_str());
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, FALSE);

        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, partial_save_result);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        curl_easy_perform(handle);
        curl_easy_reset(handle);
        rapidjson::Document document;
        document.Parse(data.c_str());
        Sleep(2000);
        assert(document.HasMember("next"));
        if (document["next"].IsNull()) {
            has_next = false;
        } else {
            next_url = document["next"].GetString();
        }
        assert(document.HasMember("results") && document["results"].IsArray());
        auto& results = document["results"];
        for (int i = 0; i < results.Size(); i++) {
            auto& result = results[i];
            assert(result.HasMember("id"));
            std::string id = std::to_string(result["id"].GetInt64());
            if (std::find(directory_contents.begin(), directory_contents.end(), id) != directory_contents.end()) {
                continue;
            }
            if (!result.HasMember("ended") || result["ended"].IsNull()) continue;
            std::cout << "Downloading game id: " << id << std::endl;

            std::string sgf_download_url = std::string("https://online-go.com/api/v1/games/") + id + "/sgf";
            curl_easy_setopt(handle, CURLOPT_URL, sgf_download_url.c_str());
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, FALSE);
            data.clear();
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, partial_save_result);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
            curl_easy_perform(handle);
            curl_easy_reset(handle);

            std::string new_file_path = game_dir + "/" + id + ".sgf";
            std::ofstream newsgf(new_file_path, std::ios::binary);
            newsgf.write(data.c_str(), data.size());
            newsgf.close();
            new_files.push_back(new_file_path);
            Sleep(2000);
        }
        ++current_page;

    }

    curl_easy_cleanup(handle);
//    std::cout << data << std::endl;
    return new_files;
}

//int main(int, char**) {
//    download_missing_games("504897", "C:/users/purti/documents/go/ogsgames");
//    get_directory_contents("C:/users/purti/documents/go/ogsgames");
//    return 0;
//}