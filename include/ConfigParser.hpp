#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "ServerConfig.hpp"
#include "Server.hpp"
#include <string>
#include <vector>

class ConfigParser {
private:
    std::vector<ServerConfig> servers;
    std::vector<std::string> server_config;
    
    ServerConfig parseServerBlock(const std::string& blockContent);
    std::vector<std::string> splitTokens(const std::string& blockContent, const std::string& delimiters);
    RouteConfig parseLocationBlock(const std::string& path, const std::vector<std::string>& location);
public:
    ConfigParser();
    ~ConfigParser();

    void parse(const std::string& filePath);
    std::string	readFile(std::string path);
    void splitServerBlocks(const std::string &content);
    size_t findServerStart(size_t start, const std::string& content);
    size_t findServerEnd(size_t start, const std::string& content);
    std::vector<ServerConfig>& getServers();
    void removeSemicoln(std::string& str);
    bool isDuplicateServer(const ServerConfig& server1, const ServerConfig& server2);

    class ErrorException : public std::exception
		{
			private:
				std::string _message;
			public:
				ErrorException(std::string message) throw()
				{
					_message = "CONFIG PARSER ERROR: " + message;
				}
				virtual const char* what() const throw()
				{
					return (_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
};

#endif