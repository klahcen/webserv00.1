#include <iostream>
#include "../include/RouteConfig.hpp"
#include <sys/stat.h>

RouteConfig::RouteConfig() {
    this->path = "";
    this->root = "";
    this->alias = "";
    this->index = "";
    this->autoIndex = false;
    this->redirection = "";
    this->allowedMethods.reserve(3);
    this->allowedMethods.push_back("GET");
    this->allowedMethods.push_back("");
    this->allowedMethods.push_back("");
}

RouteConfig::~RouteConfig() {
    // Clear STL containers (optional; they automatically free memory on destruction)
    allowedMethods.clear();
    cgiPass.clear();
    cgiExtension.clear();

    // Clear strings (optional)
    path.clear();
    root.clear();
    alias.clear();
    index.clear();
    redirection.clear();
    uploadSavePath.clear();

    // Any other custom cleanup can go here.
}

void RouteConfig::setPath(const std::string& path)
{
    this->path = path;
}

const std::string& RouteConfig::getPath() const
{
    return (this->path);
}

// bool doesPathExist(const std::string& path) {
//     struct stat buffer;
//     return (stat(path.c_str(), &buffer) == 0);
// }

void RouteConfig::setRoot(const std::string& root)
{
    if(!this->root.empty())
        this->root.clear();
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
    // if (!doesPathExist(rootValue))
    //     throw ErrorException("Root path does not exist: " + rootValue);

    this->root = rootValue;
}

const std::string& RouteConfig::getRoot() const
{
    return (root);
}

void RouteConfig::setAlias(const std::string& alias)
{
    if(!this->alias.empty())
        this->alias.clear();
    if (alias.empty() || alias[alias.size() - 1] != ';')
        throw ErrorException("Alias directive must end with ';'");
    std::string aliasValue = alias.substr(0, alias.size() - 1);
    if (aliasValue.empty())
        throw ErrorException("alias path cannot be empty");
    for (size_t i = 0; i < aliasValue.size(); ++i) {
        char c = aliasValue[i];
        if (!(std::isalnum(c) || c == '/' || c == '_' || c == '-' || c == '.')) {
            throw ErrorException("Alias path contains invalid characters: " + aliasValue);
        }
    }
    if (aliasValue[0] != '.') {
        throw ErrorException("Alias path must be an absolute path starting with '/'");
    }
    // if (!doesPathExist(aliasValue))
    //     throw ErrorException("alias path does not exist: " + aliasValue);

    this->alias = aliasValue;
}

const std::string& RouteConfig::getAlias() const
{
    return (alias);
}

void RouteConfig::setIndex(const std::string& index)
{
    if (index.empty() || index[index.size() - 1] != ';')
        throw ErrorException("Index directive must end with ';'");
    std::string indexValue = index.substr(0, index.size() - 1);
    this->index = indexValue;
}

const std::string& RouteConfig::getIndex() const
{
    return (index);
}

void RouteConfig::setAutoIndex(const std::string& autoindex)
{
    if (autoindex.empty() || autoindex[autoindex.size() - 1] != ';')
        throw ErrorException("autoIndex directive must end with ';'");
     std::string autoindexValue = autoindex.substr(0, autoindex.size() - 1);
    if (autoindexValue != "on" && autoindexValue != "off")
		throw ErrorException("Invalid 'autoindex' value (expected 'on' or 'off')");
	if (autoindexValue == "on")
		this->autoIndex = true;
}

bool RouteConfig::getAutoIndex() const
{
    return (autoIndex);
}

void RouteConfig::setAllowedMethods(const std::vector<std::string>& methods)
{
    for (size_t i = 0; i < methods.size(); i++)
    {
        if (methods[i] == "GET")
            this->allowedMethods[i] = "GET";
        else if (methods[i] == "POST")
            this->allowedMethods[i] = "POST";
        else if (methods[i] == "DELETE")
            this->allowedMethods[i] = "DELETE";
        else
            throw ErrorException("Method not supported '" + methods[i] + "'");
    }
}

const std::vector<std::string>& RouteConfig::getAllowedMethods() const
{
    return (this->allowedMethods);
}

void RouteConfig::setRedirection(const std::string& url)
{
    if (url.empty() || url[url.size() - 1] != ';')
        throw ErrorException("url directive must end with ';'");
    std::string urlValue = url.substr(0, url.size() - 1);
    this->redirection = urlValue;
}

const std::string& RouteConfig::getRedirection() const
{
    return (redirection);
}

bool RouteConfig::hasRedirect() const
{
    return (!redirection.empty());
}

void RouteConfig::setCgiPass(const std::vector<std::string>& cgi)
{
    this->cgiPass = cgi;
}

const std::vector<std::string>& RouteConfig::getCgiPass() const
{
    return (cgiPass);
}

void RouteConfig::setCgiExtension(const std::vector<std::string>& extension)
{
    this->cgiExtension = extension;
}

const std::vector<std::string>& RouteConfig::getCgiExtension() const
{
    return (cgiExtension);
}

void RouteConfig::setUploadSavePath(const std::string& path)
{
    if (path.empty() || path[path.size() - 1] != ';')
        throw ErrorException("url directive must end with ';'");
    std::string pathValue = path.substr(0, path.size() - 1);
    this->uploadSavePath = pathValue;
}

const std::string& RouteConfig::getUploadSavePath() const
{
    return (uploadSavePath);
}
