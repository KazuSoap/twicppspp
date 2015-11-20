
#if defined(_WIN32) && defined(_MSC_VER)
#include <windows.h>
#else
#include <iconv.h>
#include <errno.h>
#endif

#include <iostream>
#include <cstring>
#include <fstream>
// openssl で base64 エンコードする場合必要
// #include <openssl/buffer.h>
// #include <openssl/bio.h>
// #include <openssl/evp.h>
#include <oauth.h>
#include <json/json.h>
#include <json/reader.h>
#include <json/writer.h>
#include <opencv2/opencv.hpp>

#include "webclient.h"
#include "tweet.h"

static const char *uri_update = "https://api.twitter.com/1.1/statuses/update.json";
static const char *uri_media_upload = "https://upload.twitter.com/1.1/media/upload.json";

static bool http_request(const char *u, const char *p, std::string *reply = 0)
{
    if (reply) {
        reply->clear();
    }
    WebClient client;
    URI uri(u);
    if (p) {
        WebClient::Post post;
        post.data.assign(p, p + strlen(p));
        client.post(uri, &post);
    } else {
        client.get(uri);
    }
    WebClient::Result const &res = client.result();
    if (res.headers.size() > 0) {
        int a, b, c;
        sscanf(res.headers[0].c_str(), "HTTP/%u.%u %u", &a, &b, &c);
        if (c == 200) {
            if (reply && !res.content.empty()) {
                char const *p = (char const *)&res.content[0];
                *reply = std::string(p, p + res.content.size());
            }
            return true;
        }
    }
    return false;
}


#if defined(_WIN32) && defined(_MSC_VER)

static std::string sjis_to_utf8(std::string const &message)
{
    int n;
    wchar_t ucs2[1000];
    char utf8[1000];
    n = MultiByteToWideChar(CP_ACP, 0, message.c_str(), message.size(), ucs2, 1000);
    n = WideCharToMultiByte(CP_UTF8, 0, ucs2, n, utf8, 1000, 0, 0);
    return std::string(utf8, n);
}

#else

static std::string conv(char const *dstenc, char const *srcenc, std::string const &src)
{
    std::vector<char> out;
    iconv_t cd = iconv_open(dstenc, srcenc);
    if (cd != (iconv_t)-1) {
        char tmp[65536];
        size_t space = sizeof(tmp);
        char *inbuf = (char *)src.c_str();
        size_t inleft = src.size();
        size_t n;
        while (inleft > 0) {
            char *outbuf = tmp;
            size_t outleft = space;
            n = iconv(cd, &inbuf, &inleft, &outbuf, &outleft);
            if (n == -1 && errno != E2BIG) {
                break;
            }
            n = space - outleft;
            out.insert(out.end(), tmp, tmp + n);
        }
        iconv_close(cd);
    }
    if (out.empty()) {
        return std::string();
    }
    return std::string(&out[0], out.size());
}

#endif

static void encodeImg2Base64(cv::Mat const &src, std::string &buff_b64)
{
    // Mat データをメモリ上で画像フォーマットにエンコード
    std::vector<uchar> buff; // buffer for coding
    std::vector<int> param = { CV_IMWRITE_PNG_COMPRESSION, 3 }; // default(3)  0-9.
    cv::imencode( ".png", src, buff, param ); // png にエンコード

    // base64 encoding
    buff_b64 = oauth_encode_base64 ( buff.size(), buff.data());

    // 以下 openssl version
    // base64 encoding
    // BIO *b64 = BIO_new( BIO_f_base64() );
    // BIO_set_flags( b64, BIO_FLAGS_BASE64_NO_NL ); // 改行の無効化
    // BIO *bmem = BIO_new( BIO_s_mem() );
    // b64 = BIO_push( b64, bmem );
    // BIO_write( b64, buff.data(), buff.size() );
    // BIO_flush( b64 );

    // 結果を std::string に格納
    // BUF_MEM *bptr;
    // BIO_get_mem_ptr( b64, &bptr );

    // buff_b64 = std::string( bptr->data, bptr->length );

    // // フィルタの解放
    // BIO_free_all( b64 );
}

bool TwitterClient::tweet(std::string message, cv::Mat const &src)
{
    if( message.empty() && src.empty() ){
        return false;
    }

    // convert message to utf-8
    if( !message.empty() ){
#if defined(_WIN32) && defined (_MSC_VER)
        message = sjis_to_utf8(message);
#elif defined(_WIN32) && defined (__GNUC__)
        message = conv("utf-8", "sjis", message);
#endif
    }

    std::string uri;
    if( src.empty() ){
        uri = std::string(uri_update) + std::string("?status=") + oauth_url_escape(message.c_str());
    } else {
        std::string buff_b64;
        encodeImg2Base64( src, buff_b64);

        uri = std::string(uri_media_upload) + std::string("?media_data=") + oauth_url_escape(buff_b64.c_str());
        char* post_media_upload = NULL; // must free!!
        std::string request = oauth_sign_url2(uri.c_str(), &post_media_upload, OA_HMAC, "POST", c_key(), c_sec(), t_key(), t_sec());

        std::string reply;
        if( ! http_request(request.c_str(), post_media_upload, &reply) ){
            return false;
        }

        // メモリの解放 & ヌルポ
        free(post_media_upload);
        post_media_upload = NULL;

        Json::Reader reader;
        Json::Value j;
        reader.parse ( reply, j );

        uri = std::string(uri_update)
            + std::string("?status=") + oauth_url_escape(message.c_str())
            + std::string("?media_ids=") + oauth_url_escape( j[ "media_id_string" ].asCString() );
    }

    char* post_update = NULL; // must free!!
    std::string request = oauth_sign_url2(uri.c_str(), &post_update, OA_HMAC, "POST", c_key(), c_sec(), t_key(), t_sec());

    bool ok = http_request(request.c_str(), post_update);

    // メモリの解放 & ヌルポ
    free(post_update);
    post_update = NULL;

    return ok;
}

bool TwitterClient::tweet(std::string message, std::string filename)
{
    if( filename.empty() ){
        return false;
    }

    return tweet( message, cv::imread(filename));
}
