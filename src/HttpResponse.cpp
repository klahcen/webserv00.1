#include "../include/HttpResponse.hpp"
#include <sstream>
#include <fstream>
#include <iostream>


// Constructor
HttpResponse::HttpResponse(int statusCode, const std::string& reasonPhrase)
    : statusCode(statusCode), reasonPhrase(reasonPhrase) {}

// Setters
void HttpResponse::setStatusCode(int code, const std::string& reason)
{
    statusCode = code;
    reasonPhrase = reason;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
    headers[key] = value;
}

void HttpResponse::setBody(const std::string& bodyContent) {
    body = bodyContent;

    std::stringstream oss;
    oss << body.size();
    headers["Content-Length"] = oss.str();
}

void HttpResponse::sendRedirect(int clientSocket, const std::string& location)
{
    HttpResponse redirectResponse(301, "Moved Permanently");
    redirectResponse.setHeader("Location", location);
    redirectResponse.setHeader("Content-Length", "0"); // No body needed for redirects
    redirectResponse.sendResponse(clientSocket);
}

// Sends the response over the socket
bool HttpResponse::sendResponse(int clientSocket) const
{
    std::stringstream responseStream;

    // Construct the response line
    responseStream << "HTTP/1.1 " << statusCode << " " << reasonPhrase << "\r\n";

    // Append headers
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        responseStream << it->first << ": " << it->second << "\r\n";
    }

    // End headers and add body
    responseStream << "\r\n" << body;

    std::string responseString = responseStream.str();
    ssize_t sentBytes = write(clientSocket, responseString.c_str(), responseString.size());
    if(sentBytes<=0)
    { 
        std::cerr<<"Write failed: "<<std::endl;
    }
    return (sentBytes == static_cast<ssize_t>(responseString.size()));
}

// Static helper to send error responses
bool HttpResponse::sendError(int clientSocket, int errorCode, const std::string& message, const ServerConfig &server)
{
    const std::map<int, std::string>& errorPages = server.getErrorPages();
    std::map<int, std::string>::const_iterator it = errorPages.find(errorCode);
    if (it != errorPages.end())
    {
        // Error page found, serve the file
        std::string html = it->second.c_str();
        if(html[0] != '/')
                html.insert(html.begin(), '/');
        std::string target_file = server.getRoot() + html;
        std::ifstream errorPageFile(target_file.c_str());
        if (errorPageFile.is_open())
        {
            std::stringstream buffer;
            buffer << errorPageFile.rdbuf(); // Read the file content
            errorPageFile.close();

            HttpResponse errorResponse(errorCode, message);
            errorResponse.setHeader("Content-Type", "text/html; charset=UTF-8");
            errorResponse.setBody(buffer.str());
            return errorResponse.sendResponse(clientSocket);
        }
        else
        {
            // Error page file exists in config but cannot be opened
            std::cerr << "Error: Cannot open error page file: " << target_file << std::endl;
        }
    }

    // No custom error page, fall back to a simple HTML response
    std::stringstream htmlBody;
    htmlBody << "<!DOCTYPE html>\n"
            << "<html lang=\"en\">\n"
            << "<head>\n"
            << "    <meta charset=\"UTF-8\">\n"
            << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
            << "    <title>" << errorCode << " " << message << "</title>\n"
            << "    <style>\n"
            << "        body { font-family: Arial, sans-serif; margin: 0; padding: 0; display: flex; align-items: center; justify-content: center; height: 100vh; background-color: #f8f9fa; color: #333; }\n"
            << "        .container { text-align: center; }\n"
            << "        h1 { font-size: 3rem; margin: 0; }\n"
            << "        p { font-size: 1.25rem; margin: 10px 0; }\n"
            << "    </style>\n"
            << "</head>\n"
            << "<body>\n"
            << "    <div class=\"container\">\n"
            << "        <h1>" << errorCode << " " << message << "</h1>\n"
            << "        <p>Sorry, something went wrong. Please try again later.</p>\n"
            << "    </div>\n"
            << "</body>\n"
            << "</html>";

    HttpResponse errorResponse(errorCode, message);
    errorResponse.setHeader("Content-Type", "text/html; charset=UTF-8");
    errorResponse.setHeader("Connection","close");
    errorResponse.setBody(htmlBody.str());
    return errorResponse.sendResponse(clientSocket);
}