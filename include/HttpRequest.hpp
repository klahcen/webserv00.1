#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <cstddef>
#include <string>
#include <map>
#include <set>
#include <sys/types.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <ctime>
// #define BUF_SIZE 4048

class HttpRequest
{
private:
    std::string method;                         // HTTP method (GET, POST, etc.)
    std::string path;                           // Target path (e.g., /index.html)
    std::string version;                        // HTTP version (e.g., HTTP/1.1)
    std::map<std::string, std::string> headers; // key-value pairs for headers
    // std::string body;                           // Body of the request (if any)
    std::string nextchunk;
    std::string fileName;
    std::string type;
    long long length;
    int buffer;
    bool headersComplete;
    size_t contentLength;
    bool isChunked;
    bool chack;
    char *bodyBuffer;
    FILE *ftype;
    std::string body;
    ssize_t totalesize;
    bool checkfirsttime;
    bool first_line_complte;
    public:
    std::string path_file;
    std::fstream file_body;
    bool requestcomplet;
    int bytesRead;
    HttpRequest();
    ~HttpRequest();
    HttpRequest(const HttpRequest &other);
    HttpRequest& operator= (const HttpRequest &other);
    void postChunked(const std::string &socketStream);
    void generateRandomFileName();
    std::string get_pathfile();
    int setfirstBody();
    size_t getcontentLength();
    int getBuffer(std::string req);
    void postBinary(const std::string &req);
    bool readFromSocket(std::string &persistentBuffer);
    bool parseRequestLine(const std::string &requestLine);
    bool parseHeaders(std::istringstream &requestStream);

    // Getters for parsed data
    std::string getMethod() const;

    std::string getPath() const;
    std::string getVersion() const;
    std::string getHeader(const std::string &headerName) const;
    std::map<std::string, std::string> getHeaders()
    {
        return headers;
    }
    std::string getBody1() ;
    std::fstream& getBody() ;
    bool getMultiformFlag();
    std::string getBoundary();
    int getContentLength() const;
    std::string getContentType() const;
    // Helper function to display the request (for debugging)
    void displayRequest() const;
    std::string getQueryString() const;
};

#endif