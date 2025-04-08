#include "curl_stream.h"
#include <stdexcept>

CurlStreambuf::CurlStreambuf(const std::string &url) : transfer_done(false) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handle = curl_easy_init();
    if (!curl_handle) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,
                     &CurlStreambuf::WriteCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
}

CurlStreambuf::~CurlStreambuf() {
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
}

size_t CurlStreambuf::WriteCallback(char *ptr, size_t size, size_t nmemb,
                                    void *userdata) {
    CurlStreambuf *self = static_cast<CurlStreambuf *>(userdata);
    size_t total = size * nmemb;
    self->buffer.insert(self->buffer.end(), ptr, ptr + total);
    return total;
}

int CurlStreambuf::underflow() {
    // Check if there are characters available in the get area
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    if (transfer_done) {
        return traits_type::eof();
    }

    // Perform the transfer
    CURLcode res = curl_easy_perform(curl_handle);
    transfer_done = true;

    if (res != CURLE_OK || buffer.empty()) {
        return traits_type::eof();
    }

    // Set the get area pointers to cover the entire buffer
    char *base = buffer.data();
    setg(base, base, base + buffer.size());

    return traits_type::to_int_type(*gptr());
}

CurlStream::CurlStream(const std::string &url)
    : std::istream(&streambuf), streambuf(url) {}

CurlStream::~CurlStream() {}

bool is_url(std::string input) {
    return (input.substr(0, 7) == "http://" ||
            input.substr(0, 8) == "https://");
}
