#include "../include/CGIHandler.hpp"
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <sys/socket.h>
#include <cstring> // For strerror
#include <algorithm>
#include <ctype.h>

CGIHandler::CGIHandler() 
{
}

CGIHandler::~CGIHandler() {
}
void logError(const std::string &message)
{
    std::cerr << "[CGIHandler ERROR]: " << message << std::endl;
}

std::string generateRandomFileName() {

    std::string path_file;
  const std::string validChars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  std::string randomFileName;

  std::srand(std::time(NULL));
  for (int i = 0; i < 25; i++)
    randomFileName.push_back(validChars[rand() % validChars.length()]);

  path_file = "/tmp/";
  if (path_file.at(path_file.size() - 1) != '/')
    path_file.push_back('/');
  path_file += randomFileName + ".txt";
  return path_file;
}

char** CGIHandler::convertEnvToCharArray()
{
    char** envArray = new char*[environmentVariables.size() + 1];
    size_t i = 0;
    for (std::map<std::string, std::string>::iterator it = environmentVariables.begin(); it != environmentVariables.end(); ++it)
    {
        std::string envEntry = it->first + "=" + it->second;
        envArray[i] = new char[envEntry.size() + 1];
        std::strcpy(envArray[i], envEntry.c_str());
        i++;
    }
    envArray[i] = NULL;
    return envArray;
}
void CGIHandler::init(HttpRequest &request, const std::string &interpreter, const std::string &scriptFile, const std::map<std::string, std::string> &queryParams) {
    
    this->interp = interpreter;
    this->script = scriptFile;
    this->_request = request;
    tmpFileName = generateRandomFileName();
    setupEnvironment(request, scriptFile, queryParams);

    outFile.open(tmpFileName.c_str(),  std::ios::app);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open temporary file for CGI output");
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd) == -1) {
        throw std::runtime_error("Failed to create socket pair for CGI communication");
    }
    env = convertEnvToCharArray();
    stillRunning = false;
    isHeaderSet = false;
    lastTimeRead = time(NULL);
}

bool isNumeric(std::string &str)
{
    size_t end = str.find_last_not_of(" \t\r\n");
    if (end != std::string::npos)
        str.erase(end + 1);
    if (str.empty())
        return false;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (!std::isdigit(str[i]))
            return false;
    }
    return true;
}

std::string removewithspece(std::string value)
{
     size_t end = value.find_last_not_of(" \t\r\n");
    if (end != std::string::npos)
        value.erase(end + 1);

    size_t start = value.find_first_not_of(" \t\r\n");
    if (start != std::string::npos)
        value.erase(0, start);
    return value;
}

void CGIHandler::setupEnvironment(HttpRequest &request, const std::string &scriptFile, const std::map<std::string, std::string> &queryParams) {
    environmentVariables.clear();

    environmentVariables["GATEWAY_INTERFACE"] = "CGI/1.1";
    environmentVariables["SERVER_PROTOCOL"] = "HTTP/1.1";
    environmentVariables["REQUEST_METHOD"] = request.getMethod();
    environmentVariables["SCRIPT_FILENAME"] = scriptFile;
    environmentVariables["QUERY_STRING"] = buildQueryString(queryParams);
    environmentVariables["REDIRECT_STATUS"] = "200";
    environmentVariables["HTTP_COOKIE"] = request.getHeader("Cookie");

    if (request.getMethod() == "POST") {
        const size_t MAX_REQUEST_BODY_SIZE = 20 * 1024 * 1024;

        if (request.getBody1().size() > MAX_REQUEST_BODY_SIZE)
        {
            logError("Request body size exceeds the maximum allowed limit.");
            throw std::runtime_error("Request body too large");
        }
        inFile.open(request.get_pathfile().c_str());
        if (!inFile.is_open()) {
            throw std::runtime_error("Failed to open temporary file for CGI input");
        }
        std::stringstream oss;
            oss << request.getContentLength()<<"\n";
            std::string contentLength = oss.str();
            if (!isNumeric(contentLength))
            {
                logError("Invalid CONTENT_LENGTH: " + contentLength);
                contentLength = "0";
            }
            this->environmentVariables["CONTENT_LENGTH"] = contentLength;
            std::string contentType = request.getContentType();
            contentType.erase(0, contentType.find_first_not_of(" \t"));
            contentType.erase(contentType.find_last_not_of(" \t") + 1);

            if (contentType.empty())
            {
                contentType = "text/plain"; 
            }
            this->environmentVariables["CONTENT_TYPE"] = contentType;
    }
    else {
        this->environmentVariables["CONTENT_LENGTH"]="0";
        this->environmentVariables["CONTENT_TYPE"]="";
    }
}


std::string CGIHandler::buildQueryString(const std::map<std::string, std::string> &queryParams) {
    std::stringstream queryString;

    std::map<std::string, std::string>::const_iterator  param = queryParams.begin();
    for (;param!=queryParams.end();param++) {
        if (!queryString.str().empty()) {
            queryString << "&";
        }
        queryString << param->first << "=" << param->second;
    }
    return queryString.str();
}
void	freeArray(char** arr)
{
	for (size_t i = 0; arr[i]; i++)
		delete[] arr[i];
	delete[] arr;
}

void CGIHandler::execute() {
    pid = fork();
    if (pid == -1) {
        throw std::runtime_error("Fork failed: ");
    }

    if (pid == 0) {
        dup2(pipefd[1], STDOUT_FILENO); 
        dup2(pipefd[1], STDIN_FILENO); 

        close(pipefd[0]);
        close(pipefd[1]);

        char *args[] = {
            const_cast<char*>(this->interp.c_str()), 
            const_cast<char*>(this->script.c_str()),  
            NULL
        };

        execve(this->interp.c_str(), args, env);
        freeArray(env);
        std::cerr << "Execve failed: "<< std::endl;
        exit(EXIT_FAILURE);
    }
    {
        stillRunning = true;
        lastTimeRead = time(NULL);
        freeArray(env);
        close(pipefd[1]);

        epoll_fd = epoll_create(1);
        if (epoll_fd == -1) {
            killChild();
            throw std::runtime_error("Failed to create epoll instance: ");
        }

        event.events = EPOLLIN | EPOLLOUT;
        event.data.fd = pipefd[0];
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipefd[0], &event) == -1) {
            killChild();
            throw std::runtime_error("Failed to add file descriptor to epoll: ");
        }
    }
}

void CGIHandler::killChild() {
    if (pid > 0) {
        kill(pid, SIGTERM);
        sleep(1);
        if (waitpid(pid, NULL, WNOHANG) == 0) {
            kill(pid, SIGKILL);
        }
        int status;
        waitpid(pid, &status, 0);
        stillRunning = false;
    }
}
void clearFile(const std::string &filePath) {
    std::fstream file(filePath.c_str(), std::ios::out | std::ios::trunc);
    
    if (!file) {
        std::cerr << "Error opening file: " << filePath << std::endl;
    }

    file.close();
}


int CGIHandler::readWrite() {
    const int BUFFER_SIZE = 4098; 
    char buffer[BUFFER_SIZE];
    struct epoll_event events[1];
    int numEvents;
    numEvents = epoll_wait(epoll_fd, events, 1, 3000);
    if (numEvents == -1) {
        throw std::runtime_error("epoll_wait failed: ");
    }
    if (numEvents > 0) {
        if (events[0].events & EPOLLIN) { 
            ssize_t bytesRead = read(pipefd[0], buffer, BUFFER_SIZE - 1);
            
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0'; 
                cgiBuffer.append(buffer, bytesRead);
                lastTimeRead = time(NULL);
            }
        }
        else if (events[0].events & EPOLLOUT) {
            inFile.read(buffer, BUFFER_SIZE - 1);
            ssize_t bytesRead = inFile.gcount();

            if (bytesRead > 0) {
                buffer[bytesRead] = '\0'; 
                if(send(pipefd[0], buffer, inFile.gcount(), MSG_NOSIGNAL)<=0)
                { 
                    throw std::runtime_error("Write failed: ");
                }
            }
        }
    }
    if (!stillRunning&&!isHeaderSet) {
        size_t headerEnd = cgiBuffer.find("\r\n\r\n");
        int i = 4;
        if(headerEnd==std::string::npos)
        {
            
            headerEnd = cgiBuffer.find("\r\n");
            if(headerEnd==std::string::npos)
            {
                throw  std::runtime_error("Failed");
            }
            i = 2;
        }
        if (headerEnd != std::string::npos) {
            std::string headers = cgiBuffer.substr(0, headerEnd);
            cgiBuffer.erase(0, headerEnd+i); 
            isHeaderSet = true;

            std::istringstream headerStream(headers);
            std::string line;
            while (std::getline(headerStream, line)) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = line.substr(0, colonPos);
                    std::string value = line.substr(colonPos + 1);
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    cgiHeaders[key] = value; 
                }
            }
        }
        
        
    }

    if (isHeaderSet && !cgiBuffer.empty()) {
        clearFile(tmpFileName);
        outFile << cgiBuffer<<"\n";
        std::string line;
        outFile.seekg(0, std::ios::beg);
        cgiBuffer.clear();
        cgi_is_complet = true;
        return (1);
    }
    if (checkTimeout()) {
        killChild();
        throw std::runtime_error("CGI process timed out");
    }
    return (stillRunning ? 0 : 1);
}

bool CGIHandler::checkTimeout() const {
    const int CGI_TIMEOUT = 10;
    return (time(NULL) - lastTimeRead > CGI_TIMEOUT);
}

int returnEXitStatus(int status)
{
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		return WTERMSIG(status);
	else
		return 0;
}

void CGIHandler::waitForCgi()
{
	if (!stillRunning)
		return ;

	int status;
	int w = waitpid(pid, &status, WNOHANG);
	if (w == -1)
	{
		killChild();
		throw std::runtime_error("CGI process w = -1");
	}
	else if (w)
	{
		if (returnEXitStatus(status))
		{
		    killChild();
			throw std::runtime_error("CGI process filed");
		}
		stillRunning = false;
	}
	return ;
}


int CGIHandler::getStatusCode() const {
    if (cgiHeaders.find("Status") != cgiHeaders.end()) {
        std::string status = cgiHeaders.at("Status");
        return std::atoi(status.c_str()); 
    }
    return 200; 
}

std::string CGIHandler::getBodyFileName() const {
    return tmpFileName;
}


void CGIHandler::cleaning() {
    if (epoll_fd != -1) {
        close(epoll_fd); 
    }
    if (pipefd[0] != -1) {
        close(pipefd[0]);
    }
    if (outFile.is_open()) {
        outFile.close(); 
    }
    if (inFile.is_open()) {
        inFile.close(); 
    }
    if (!tmpFileName.empty()) {
        unlink(tmpFileName.c_str()); 
    }
}

const std::map<std::string, std::string> &CGIHandler::getHeaders() const {
    return cgiHeaders;
}
