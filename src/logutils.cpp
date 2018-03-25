#include "logutils.h"

#include <ctime>

std::string get_timestamp() {
    char timestamp[20];
    time_t now = time(nullptr);
    strftime(timestamp + 1, 19, "%d/%m/%y %H:%M:%S", localtime(&now));
    timestamp[0] = '[';
    timestamp[sizeof(timestamp)/sizeof(char) - 1] = '\0';
    timestamp[sizeof(timestamp)/sizeof(char) - 2] = ']';
    return std::string(timestamp);
}

size_t partial_save_result(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* whole_result = reinterpret_cast<std::string*>(userdata);
    std::string partial(ptr, size * nmemb);
    *whole_result += partial;
    return size * nmemb;
}
