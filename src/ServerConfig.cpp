#include "../include/ServerConfig.hpp"
#include <climits>
#include <sstream>
#include <sys/stat.h>
#include <iostream>

ServerConfig::ServerConfig()
{
    this->_port = 0;
	this->_host = "";
    this->_default_root = "";
    this->_serverName = "";
    this->_clientMaxBodySize = 1000000000;
    this->_index = "";
    this->_autoIndex = false;
}

ServerConfig::~ServerConfig() {}    

size_t ft_stoi(const std::string &str)
{
    size_t result = 0;
    size_t i = 0;

    if (str[i] == '+')
        ++i;
    for (; i < str.size(); i++)
    {
        if (!std::isdigit(str[i]))
            throw std::exception();
        int digit = str[i] - '0';
        // if (result > (INT_MAX - digit) / 10)
        // {
        //     std::cout << "client max body size biger than max" << std::endl;
        //     throw std::exception();
        // }
        result = result * 10 + digit;
    }
    return (result);
}

void ServerConfig::setPort(const std::string& port)
{
    if (port.empty() || port[port.size() - 1] != ';')
        throw ErrorException("Port directive must end with ';'");
    
    std::string portValue = port.substr(0, port.size() - 1);

    for (size_t i = 0; i < portValue.size(); i++)
    {
        if (!std::isdigit(portValue[i]))
            throw ErrorException("Port must be a numeric value");
    }

    int portNumber = ft_stoi(portValue);

    if (portNumber < 1 || portNumber > 65536)
        throw ErrorException("Port number must be between 1 and 65536");
    
    this->_port = portNumber;
}

int ServerConfig::getPort() const
{
    return (this->_port);
}

bool isValidIPv4(const std::string& ip)
{
    std::istringstream stream(ip);
    std::string segment;
    int octectCount = 0;

    while (std::getline(stream, segment, '.'))
    {
        if (++octectCount > 4)
            return (false);
        for (size_t i = 0; i < segment.size(); i++)
        {
            if (!std::isdigit(segment[i]))
                return (false);
        }
        int octet = ft_stoi(segment);
        if (octet < 0 || octet > 255)
            return(false);
    }
    return (octectCount == 4);
}

void ServerConfig::setHost(const std::string& host)
{
    if (host.empty() || host[host.size() - 1] != ';')
        throw ErrorException("Host directive must end with ';'");
    std::string hostValue = host.substr(0, host.size() - 1);
    if (hostValue == "localhost")
		hostValue = "127.0.0.1";
    if (isValidIPv4(hostValue))
    {
        this->_host = hostValue;
        return ;
    }
    throw ErrorException("Invalid host: must be a valid IPv4 address or domain name");
}

const std::string& ServerConfig::getHost() const
{
    return (this->_host);
}

bool doesPathExist(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

void ServerConfig::setRoot(const std::string& root) {
    if (root.empty() || root[root.size() - 1] != ';')
        throw ErrorException("Root directive must end with ';'");
    std::string rootValue = root.substr(0, root.size() - 1);
    if (rootValue.empty())
        throw ErrorException("Root path cannot be empty");
    for (size_t i = 0; i < rootValue.size(); ++i) {
        char c = rootValue[i];
        if (!(std::isalnum(c) || c == '/' || c == '_' || c == '-' || c == '.')) {
            throw ErrorException("Root path contains invalid characters: " + rootValue);
        }
    }
    if (rootValue[0] != '.') {
        throw ErrorException("Root path must be an absolute path starting with '/'");
    }
    this->_default_root = rootValue;
}

const std::string& ServerConfig::getRoot() const
{
    return (this->_default_root);
}

void ServerConfig::setServerName(const std::string& name)
{
    if (name.empty() || name[name.size() - 1] != ';')
        throw ErrorException("Server_name directive must end with ';'");
    std::string nameValue = name.substr(0, name.size() - 1);
    this->_serverName = nameValue;
}

const std::string& ServerConfig::getServerName() const
{
    return (this->_serverName);
}

void ServerConfig::setClientMaxBodySize(const std::string& size)
{
    if (size.empty() || size[size.size() - 1] != ';')
        throw ErrorException("Client_Size directive must end with ';'");
    std::string sizeValue = size.substr(0, size.size() - 1);
    for (size_t i = 0; i < sizeValue.size(); i++)
	{
		if (!isdigit(sizeValue[i]))
        {
            throw ErrorException("Invalid value for 'client_max_body_size', should be a number");
        }
	}
    size_t maxBodySize = ft_stoi(sizeValue);

    if (maxBodySize > (500 * 1024 * 1024))
    {
        throw ErrorException("Invalid value for 'client_max_body_size', too big");
    }
    
	this->_clientMaxBodySize = maxBodySize;
}

size_t ServerConfig::getClientMaxBodySize() const
{
    return (this->_clientMaxBodySize);
}

void ServerConfig::setIndex(const std::string& index)
{
    if (index.empty() || index[index.size() - 1] != ';')
        throw ErrorException("Index directive must end with ';'");
    std::string indexValue = index.substr(0, index.size() - 1);
    this->_index = indexValue;
}

const std::string& ServerConfig::getIndex() const
{
    return (this->_index);
}

void ServerConfig::set_Autoindex(const std::string& autoindex)
{
    if (autoindex.empty() || autoindex[autoindex.size() - 1] != ';')
        throw ErrorException("autoIndex directive must end with ';'");
    std::string autoindexValue = autoindex.substr(0, autoindex.size() - 1);
    if (autoindexValue != "on" && autoindexValue != "off")
		throw ErrorException("Invalid 'autoindex' value (expected 'on' or 'off')");
	if (autoindexValue == "on")
		this->_autoIndex = true;
}

bool  ServerConfig::get_Autoindex() const
{
    return (this->_autoIndex);
}

void ServerConfig::setErrorPages(const std::vector<std::string>& pages)
{
    if (pages.size() % 2 != 0)
    {
            throw ErrorException("Incomplete 'error_page' directive");
    }
    for (size_t i = 0; i < pages.size(); i++)
    {
        const std::string& codeStr = pages[i];
        for (size_t j = 0; j < codeStr.size(); j++)
        {
            if (!isdigit(codeStr[j])) {
                throw ErrorException("Error code must be numeric: " + codeStr);
            }
        }
        if (codeStr.size() != 3)
            throw ErrorException("Error code must be 3 characters: " + codeStr);
        int errorCode = ft_stoi(codeStr);
        if (errorCode < 400 || errorCode > 599) {
            throw ErrorException("Error code out of range: " + codeStr);
        }
        const std::string& path = pages[++i];
        if (path.empty() || path[path.size() - 1] != ';') {
            throw ErrorException("Error page path must end with a semicolon: " + path);
        }
        std::string processedPath = path.substr(0, path.size() - 1);
        this->_errorPages[errorCode] = processedPath;
    }
}

const std::map<int, std::string>& ServerConfig::getErrorPages() const
{
    return (this->_errorPages);
}

void ServerConfig::addRoute(const RouteConfig& route)
{
    this->routes.push_back(route);
}

const std::vector<RouteConfig>& ServerConfig::getRoutes() const
{
    return (routes);
}

const std::vector<RouteConfig>& ServerConfig::getRoute() const
{
    return (routes);
}