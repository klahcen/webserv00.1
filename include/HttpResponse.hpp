#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "../include/ServerConfig.hpp"
#include <string>
#include <map>
#include <unistd.h>

class HttpResponse
{
private:
    int statusCode;								// HTTP status code
    std::string reasonPhrase;					// Reason phrase (e.g., "OK", "Not Found")
    std::map<std::string, std::string> headers; // Headers as key-value pairs
    std::string body;							// Body of the response

public:
	HttpResponse(int statusCode = 200, const std::string& reasonPhrase = "OK");

	// Setters
	void setStatusCode(int code, const std::string& reason);
	void setHeader(const std::string& key, const std::string& value);
	void setBody(const std::string& bodyContent);

	// Sends the response over the socket
	static void sendRedirect(int clientSocket, const std::string& location);
	bool sendResponse(int clientSocket) const;

	// Static helper to send error responses
	static bool sendError(int clientSocket, int errorCode, const std::string& message, const ServerConfig &server);
};

#endif