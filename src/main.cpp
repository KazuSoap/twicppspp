#include <iostream>
#include <oauth.h>
#include <opencv2/opencv.hpp>

#include "tweet.h"
#include "webclient.h"
#include "mykey.h"

static void str_append(std::string *to, std::string const &s)
{
    if (!to->empty()) {
        *to += ' ';
    }
    *to += s;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        return 1;
    }

    std::string message;
    {
        bool f_stdin = false;
        bool f_base64 = false;
        bool f_separator = false;
        int i = 1;
        while (i < argc) {
            std::string arg = argv[i];
            i++;
            if (f_separator) {
                str_append(&message, arg);
            } else if (arg[0] == '-') {
                if (arg == "--") {
                    f_separator = true;
                } else if (arg == "-stdin") {
                    f_stdin = true;
                } else if (arg == "-base64") {
                    f_base64 = true;
                } else {
                    fprintf(stderr, "unknown option: %s\n", arg.c_str());
                }
            } else {
                if (f_base64) {
                    message += arg;
                } else {
                    str_append(&message, arg);
                }
            }
        }
        if (f_stdin) {
            char tmp[1000];
            int n = fread(tmp, 1, sizeof(tmp), stdin);
            message = std::string(tmp, n);
        }
        if (f_base64) {
            oauth_decode_base64((unsigned char*)message.c_str(), message.c_str());
        }
    }

    WebClient::initialize();
    TwitterClient tc(c_key, c_sec, t_key, t_sec);
    cv::Mat src = cv::imread("../../syncer.jpg");
    // std::string filename("../../syncer.jpg");

    return ( tc.tweet(message, src) ) ? 0 : 1;
    // return ( tc.tweet( message, cv::Mat() ) ) ? 0 : 1;
    // return ( tc.tweet( "", src) ) ? 0 : 1;
    // return ( tc.tweet(message, filename) ) ? 0 : 1;
    // return ( tc.tweet( "", filename) ) ? 0 : 1;
}
