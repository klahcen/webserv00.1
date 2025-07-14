#ifndef ROUTECONFIG_HPP
#define ROUTECONFIG_HPP

#include <string>
#include <vector>

class RouteConfig {
private:
    std::string path;
    std::string root;
    std::string alias;
    std::string index;
    bool autoIndex;
    std::vector<std::string> allowedMethods;
    std::string redirection;
    std::vector<std::string> cgiPass;
    std::vector<std::string> cgiExtension;
    std::string uploadSavePath;

public:
    RouteConfig();
    ~RouteConfig();

    // Getters and setters for each attribute
    void setPath(const std::string& path);
    const std::string& getPath() const;

    void setRoot(const std::string& root);
    const std::string& getRoot() const;

    void setAlias(const std::string& alias);
    const std::string& getAlias() const;

    void setIndex(const std::string& index);
    const std::string& getIndex() const;

    void setAutoIndex(const std::string& autoindex);
    bool getAutoIndex() const;

    void setAllowedMethods(const std::vector<std::string>& methods);
    const std::vector<std::string>& getAllowedMethods() const;

    void setRedirection(const std::string& url);
    const std::string& getRedirection() const;
    bool hasRedirect() const;

    void setCgiPass(const std::vector<std::string>& cgi);
    const std::vector<std::string>& getCgiPass() const;

    void setCgiExtension(const std::vector<std::string>& extension);
    const std::vector<std::string>& getCgiExtension() const;

    void setUploadSavePath(const std::string& path);
    const std::string& getUploadSavePath() const;

    class ErrorException : public std::exception
	{
		private:
			std::string _message;
		public:
			ErrorException(std::string message) throw()
			{
				_message = "LOCATION CONFIG ERROR: " + message;
			}
			virtual const char* what() const throw()
			{
				return (_message.c_str());
			}
			virtual ~ErrorException() throw() {}
	};
};

#endif