#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "RouteConfig.hpp"
#include <string>
#include <vector>
#include <map>

class ServerConfig
{
private:
    int _port;
    std::string _host;
    std::string _default_root;
    std::string _serverName;
    size_t _clientMaxBodySize;
    std::string _index;
    bool _autoIndex;
    std::map<int, std::string> _errorPages;
    std::vector<RouteConfig> routes;

public:
    ServerConfig();
    ~ServerConfig();

    // void defaultErrorPages(void);
    //Getters and setters for each attribute
    void setPort(const std::string &port);
    int getPort() const;
    void setHost(const std::string& host);
    const std::string& getHost() const;
    void setRoot(const std::string& root);
    const std::string& getRoot() const;
    void setServerName(const std::string& names);
    const std::string& getServerName() const;
    void setClientMaxBodySize(const std::string& size);
    size_t getClientMaxBodySize() const;
    void setIndex(const std::string& root);
    const std::string& getIndex() const;

    void set_Autoindex(const std::string& autoindex);
    bool get_Autoindex() const;

    void setErrorPages(const std::vector<std::string>& pages);
    // bool isValidErrorPages() const;
    const std::map<int, std::string>& getErrorPages() const;

    void addRoute(const RouteConfig& route);
    const std::vector<RouteConfig>& getRoutes() const;
    const std::vector<RouteConfig>& getRoute() const;

	class ErrorException : public std::exception
	{
		private:
			std::string _message;
		public:
			ErrorException(std::string message) throw()
			{
				_message = "SERVER CONFIG ERROR: " + message;
			}
			virtual const char* what() const throw()
			{
				return (_message.c_str());
			}
			virtual ~ErrorException() throw() {}
	};
};
    

#endif