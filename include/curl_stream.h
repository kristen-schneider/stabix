#pragma once

#include <curl/curl.h>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

class CurlStreambuf : public std::streambuf {
  public:
    explicit CurlStreambuf(const std::string &url);
    ~CurlStreambuf() override;

  protected:
    int underflow() override;

  private:
    static size_t WriteCallback(char *ptr, size_t size, size_t nmemb,
                                void *userdata);
    void FetchData();

    CURL *curl_handle;
    std::vector<char> buffer;
    size_t buffer_pos;
    bool transfer_done;
};

class CurlStream : public std::istream {
  public:
    explicit CurlStream(const std::string &url);
    ~CurlStream();

  private:
    CurlStreambuf streambuf;
};

bool is_url(std::string input);
