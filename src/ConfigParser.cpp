#include "../include/ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <iterator>
#include <vector>
#include <map>

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

/* reading from file to string */
std::string	ConfigParser::readFile(std::string path)
{
	if (path.empty() || path.length() == 0)
		throw ErrorException("File path should not be empty");
	std::ifstream config_file(path.c_str());
	if (!config_file || !config_file.is_open())
		throw ErrorException("File does not exist or could not open");

	std::stringstream stream_binding;
	stream_binding << config_file.rdbuf();
	return (stream_binding.str());
}

// remove comments from char # to \n
void removeComments(std::string &content)
{
    size_t pos;

    pos = content.find('#');
    while (pos != std::string::npos)
    {
        size_t pos_end;
        pos_end = content.find('\n', pos);
        content.erase(pos, pos_end - pos);
        pos = content.find('#');
    }
}

void removeWhiteSpaces(std::string &content)
{
    size_t i = 0;

    while (content[i] && isspace(content[i]))
        i++;
    content = content.substr(i);
    i = content.length() - 1;
    while (i > 0 && isspace(content[i]))
        i--;
    content = content.substr(0, i + 1);
}

// Helper function to find the start of a server block
size_t ConfigParser::findServerStart(size_t start, const std::string& content)
{
    size_t i = start;

    while (i < content.length())
    {
        if (isspace(content[i]))
        {
            ++i;
            continue;
        }
        // checking the "server" keyword
        if (content.compare(i, 6, "server") == 0)
        {
            i += 6; // Moving past the "server" keyword
            // Skiping whitespace after "server" keyword
            while (i < content.length() && isspace(content[i]))
                i++;
            // checking if the next character is '{'
            if (content[i] == '{')
                return (i); // Returning the position of '{'
            else
                throw ErrorException("Unexpected character after 'server'. Expected '{'.");
        }
        if (!isspace(content[i]))
            throw ErrorException("Unexpected character outside 'server' scope.");
    }
    return (std::string::npos); // No more server blocks found
}

// Helper function to find the end of a server block
size_t ConfigParser::findServerEnd(size_t start, const std::string& content)
{
    size_t i = start + 1; // Start searching after the opening brace
    int scope = 0;

    while (i < content.length())
    {
        if (content[i] == '{')
            ++scope; // Entering a nested block
        else if (content[i] == '}')
        {
            if (scope == 0)
                return (i); // Found the closing matching brace
            --scope; // Exiting a nested block
        }
        ++i;
    }
    // If no matching closing brace is found
    throw ErrorException("Unmatched braces in the 'server' block.");
}

void ConfigParser::splitServerBlocks(const std::string &content)
{
    // Check if "server" exists in the content
    size_t start = 0;
    size_t end = 0;

    // Check if "server" exists in the content
    if (content.find("server") == std::string::npos) {
        throw ErrorException("No 'server' blocks found in the configuration content.");
    }

    while (start < content.length())
    {
        // Find the start of a server block
        start = findServerStart(start, content);
        if (start == std::string::npos)
            break;
        // find the end of the current server block
        end = findServerEnd(start, content);
        if (start == end)
            throw ErrorException("Unmatched or improper scope for 'server' block.");
        // Extract the server block and add it to the vector
        this->server_config.push_back(content.substr(start, end - start + 1));
        start = end + 1; // Move the the next block
    }
}

bool ConfigParser::isDuplicateServer(const ServerConfig& server1, const ServerConfig& server2)
{
    // Compare ports
    if (server1.getPort() != server2.getPort())
        return false;

    // Compare hosts
    if (server1.getHost() != server2.getHost())
        return false;

    // Compare server names
    if (server1.getServerName() != server2.getServerName())
        return false;

    return true;
}

void ConfigParser::parse(const std::string& filePath)
{
    std::string		content;
    content = readFile(filePath);

    if (content.empty())
        throw ErrorException("Error in the config file");
    removeComments(content);
    removeWhiteSpaces(content);
    splitServerBlocks(content);

    for (size_t i = 0; i < this->server_config.size(); i++) 
    {
        ServerConfig serverConfig = parseServerBlock(this->server_config[i]);
        for (size_t j = 0; j < servers.size(); j++) 
        {
            if (isDuplicateServer(serverConfig, servers[j]))
                throw ErrorException("Duplicate server configuration: Same port, IP, and server_name");
        }
        servers.push_back(serverConfig);
    }
}

std::vector<std::string> ConfigParser::splitTokens(const std::string& blockContent, const std::string& delimiters)
{
    std::vector<std::string> result;
    std::string token;
    std::string::size_type pos = 0, start = 0;
    while ((pos = blockContent.find_first_of(delimiters, start)) != std::string::npos)
    {
        if (pos > start)
        {
            token = blockContent.substr(start, pos - start);
            result.push_back(token);
        }
        start = pos + 1;
    }

    // Add the last token if any
    if (start < blockContent.length())
    {
        token = blockContent.substr(start);
        result.push_back(token);
    }
    return (result);
}

ServerConfig ConfigParser::parseServerBlock(const std::string& blockContent)
{
    ServerConfig server;
    std::vector<std::string> tokens = splitTokens(blockContent + ' ', " \n\t");
    std::vector<std::string> errorPages;
    std::set<std::string> locationPaths;
    bool autoindexSet = false;
    bool maxBodySizeSet = false;
    int scope = 1;

    if (tokens.size() < 3)
        throw ErrorException("Insufficient server block parameters");
    for (size_t i = 0; i < tokens.size(); i++)
    {
        const std::string token = tokens[i];

        if (token == "listen" && scope)
        {
            if ((i + 1) >= tokens.size())
                throw ErrorException("Missing value for 'listen");
            if (server.getPort())
                throw ErrorException("Dublicate 'listen' directive");
            server.setPort(tokens[++i]);
        }
        else if (token == "host" && scope)
        {
            if ((i + 1) >= tokens.size())
                throw ErrorException("Missing value for 'host'");
            if (!server.getHost().empty())
                throw ErrorException("Duplicate 'host' directive");
            server.setHost(tokens[++i]);
        }
        else if (token == "default_root" && scope)
        {
            if ((i + 1) >= tokens.size())
                throw ErrorException("Missing value for 'root'");
            if (!server.getRoot().empty())
                throw ErrorException("Duplicate 'root' directive");
            server.setRoot(tokens[++i]);
        }
        else if (token == "error_page" && scope)
        {
            if ((i + 1) >= tokens.size())
                throw ErrorException("Missing value for 'error_page'");
            while (++i < tokens.size())
			{
				errorPages.push_back(tokens[i]);
				if (tokens[i].find(';') != std::string::npos)
					break ;
				if (i + 1 >= tokens.size())
					throw ErrorException("Wrong character out of server scope{}");
			} 
        }
        else if (token == "client_max_body_size" && scope)
        {
            if ((i + 1) >= tokens.size())
                throw ErrorException("Missing value for 'client_max_body_size'");
            if (maxBodySizeSet)
                throw ErrorException("Duplicate 'client_max_body_size' directive");
            server.setClientMaxBodySize(tokens[++i]);
            maxBodySizeSet = true;
        }
        else if (token == "server_name" && scope)
        {
            if ((i + 1) >= tokens.size())
                throw ErrorException("Missing value for 'server_name'");
            server.setServerName((tokens[++i]));
        }
        else if (token == "index" && scope)
        {
            if ((i + 1) >= tokens.size())
                throw ErrorException("Missing value for 'index'");
            if (!server.getIndex().empty())
                throw ErrorException("Duplicate 'index' directive");
            server.setIndex(tokens[++i]);
        }
        else if (token == "autoindex" && scope) {
            if ((i + 1) >= tokens.size())
                throw ErrorException("Missing value for 'autoindex'");
            if (autoindexSet)
                throw ErrorException("Duplicate 'autoindex' directive");
            server.set_Autoindex(tokens[++i]);
            autoindexSet = true;
        }
        else if (token == "location")
        {
            if ((i + 2) >= tokens.size() || tokens[i + 2] != "{")
            {
                throw ErrorException("Malformed 'location' block");
            }
            std::string locationPath = tokens[++i];
             // Check for duplicate location paths
            if (!locationPaths.insert(locationPath).second)
                throw ErrorException("Duplicate 'location' path: " + locationPath);
            std::vector<std::string> locationDirectives;
            for (++i; i < tokens.size() && tokens[i] != "}"; i++)
                locationDirectives.push_back(tokens[i]);
            if (i >= tokens.size() || tokens[i] != "}")
                throw ErrorException("Missing closing '}' for 'location' block");
            RouteConfig route = parseLocationBlock(locationPath, locationDirectives);
            server.addRoute(route);
            scope = 0;
        }
        else if (token != "{" && token != "}")
         {
            if (!scope)
                throw  ErrorException("Server token after location");
            else
                throw ErrorException("Unsupported directive: " + token);
        }
    }
    if (!server.getPort())
        throw ErrorException("Missing 'listen' directive");
    if (server.getRoot().empty())
        server.setRoot("/;");
    if (server.getHost().empty())
        server.setHost("localhost;");
    server.setErrorPages(errorPages);
    return (server);
}

void ConfigParser::removeSemicoln(std::string& str)
{
    if (str.empty() || str[str.size() - 1] != ';')
    {
        throw ErrorException(str + " directive must end with ';'");
    }
	str = str.substr(0, str.size() - 1);
}

RouteConfig ConfigParser::parseLocationBlock(const std::string& path, const std::vector<std::string>& locationDirectives)
{
    RouteConfig route;
    route.setPath(path);

    std::vector<std::string>::const_iterator it = locationDirectives.begin();
    bool rootSet = false, indexSet = false, autoindexSet = false, methodsSet = false, extSet = false, pathSet = false, aliasSet = false;

    for (; it != locationDirectives.end(); it++)
    {
        const std::string& directive = *it;

        if (directive == "root")
        {
            if (rootSet || aliasSet)
                throw ErrorException("Duplicate 'root' directive or 'alias' in location block");
            if (++it == locationDirectives.end())
                throw ErrorException("Missing value for 'root' in location block");
            route.setRoot(*it);
            rootSet = true;
        }
        else if (directive == "alias")
        {
            if (aliasSet || rootSet)
                throw ErrorException("Duplicate 'alias' directive or 'root' in location block");
            if (++it == locationDirectives.end())
                throw ErrorException("Missing value for 'alias' in location block");
            route.setAlias(*it);
            aliasSet = true;
        }
        else if (directive == "index")
        {
            if (indexSet)
                throw ErrorException("Duplicate 'index' directive in location block");
            if (++it == locationDirectives.end())
                throw ErrorException("Missing value for 'index' in location block");
            route.setIndex(*it);
            indexSet = true;
        }
        else if (directive == "autoindex")
        {
            if (autoindexSet)
                throw ErrorException("Duplicate 'autoindex' directive in location block");
            if (++it == locationDirectives.end())
                throw ErrorException("Missing value for 'autoindex' in location block");
            route.setAutoIndex(*it);
            autoindexSet = true;
        }
        else if (directive == "allowed_methods")
        {
            std::vector<std::string> allowed_methods;
            if (methodsSet)
                throw ErrorException("Duplicate 'allowed_methods' directive in location block");
            if (++it == locationDirectives.end())
                throw ErrorException("Missing value for 'allowed_methods' in location block");
            while (it < locationDirectives.end())
            {
                std::string str = *it;
                if (str.find(";") != std::string::npos)
                {
                    removeSemicoln(str);
                    allowed_methods.push_back(str);
                    break ;
                }
                else
                    allowed_methods.push_back(str);
                it++;
            }
            route.setAllowedMethods(allowed_methods);
            methodsSet = true;
        }
        else if (directive == "return")
        {
            if (!route.getRedirection().empty())
                throw ErrorException("Duplicate 'return' directive in location block");
            if (++it == locationDirectives.end())
                throw ErrorException("Missing value for 'redirection' in location block");
            route.setRedirection(*it);
        }
        else if (directive == "cgi_pass")
        {
            std::vector<std::string> cgi_paths;
            if (pathSet)
                throw ErrorException("Duplicate 'cgi_pass' directive in location block");
            if (++it == locationDirectives.end())
                throw ErrorException("Missing value for 'cgi_pass' in location block");
            while (it < locationDirectives.end())
            {
                std::string str = *it;
                if (str.find(";") != std::string::npos)
                {
                    removeSemicoln(str);
                    cgi_paths.push_back(str);
                    break ;
                }
                else
                    cgi_paths.push_back(str);
                it++;
            }
            route.setCgiPass(cgi_paths);
            pathSet = true;
        }
        else if (directive == "cgi_extension")
        {
            std::vector<std::string> cgi_extensions;
            if (extSet)
                throw ErrorException("Duplicate 'cgi_extension' directive in location block");
            if (++it == locationDirectives.end())
                throw ErrorException("Missing value for 'cgi_extension' in location block");
            while (it < locationDirectives.end())
            {
                std::string str = *it;
                if (str.find(";") != std::string::npos)
                {
                    removeSemicoln(str);
                    cgi_extensions.push_back(str);
                    break ;
                }
                else
                    cgi_extensions.push_back(str);
                it++;
            }
            route.setCgiExtension(cgi_extensions);
            extSet = true;
        }
        else if (directive == "upload_save_path")
        {
            if (++it == locationDirectives.end())
                throw ErrorException("Missing value for 'upload_save_path' in location block");
            route.setUploadSavePath(*it);
        }
        else if (*it != "{" && *it != "}")
        {
            throw ErrorException("Unsupported directive in location block: " + directive);
        }
    }

    return route;
}

std::vector<ServerConfig>& ConfigParser::getServers()
{
    return (servers);
}
