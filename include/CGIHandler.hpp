#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "HttpRequest.hpp"

#include <map>
#include <string>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdexcept>

class HttpRequest; // Forward declaration (assuming HttpRequest is defined elsewhere)

class CGIHandler {
public:
    CGIHandler();
    ~CGIHandler();

    // Initialization
    void init(HttpRequest &request, const std::string &interpreter, const std::string &scriptFile, const std::map<std::string, std::string> &queryParams);

    // Execution and I/O handling
    void execute();
    int readWrite();
    void waitForCgi();

    // Cleanup
    void cleaning();

    // Getters
    std::string getBodyFileName() const;
    int getStatusCode() const;
    bool checkTimeout() const;
    const std::map<std::string, std::string> &getHeaders() const;
    char** convertEnvToCharArray();

    // Utility
    std::fstream outFile;
    void killChild();
    int i;
    bool first_time;
    bool cgi_is_complet;
    bool stillRunning;
private:
    // Environment setup
    void setupEnvironment(HttpRequest &request, const std::string &scriptFile, const std::map<std::string, std::string> &queryParams);
    // std::string generateRandomName();
    std::string buildQueryString(const std::map<std::string, std::string> &queryParams);

    // State variables
    bool isHeaderSet;
    char **env;
    pid_t pid;
    int pipefd[2];
    int epoll_fd;
    struct epoll_event event;
    std::string cgiBuffer;
    std::map<std::string, std::string> cgiHeaders; // Store CGI headers
    std::string tmpFileName;
    std::ifstream inFile;
    time_t lastTimeRead;
    std::string interp;
    std::string script;
    HttpRequest _request;

    // Environment variables
    std::map<std::string, std::string> environmentVariables;
};

#endif // CGI_HANDLER_HPP
