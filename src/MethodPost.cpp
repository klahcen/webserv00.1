#include "../include/CGIHandler.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/HttpResponse.hpp"
#include "../include/Method.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

std::map<std::string, std::string> createMimeTypeMap() {
  std::map<std::string, std::string> mimeMap;

  mimeMap[".html"] = "text/html";
  mimeMap[".htm"] = "text/html";
  mimeMap[".shtml"] = "text/html";
  mimeMap[".css"] = "text/css";
  mimeMap[".xml"] = "text/xml";
  mimeMap[".gif"] = "image/gif";
  mimeMap[".jpeg"] = "image/jpeg";
  mimeMap[".jpg"] = "image/jpeg";
  mimeMap[".js"] = "application/javascript";
  mimeMap[".atom"] = "application/atom+xml";
  mimeMap[".rss"] = "application/rss+xml";
  mimeMap[".mml"] = "text/mathml";
  mimeMap[".txt"] = "text/plain";
  mimeMap[".jad"] = "text/vnd.sun.j2me.app-descriptor";
  mimeMap[".wml"] = "text/vnd.wap.wml";
  mimeMap[".htc"] = "text/x-component";
  mimeMap[".avif"] = "image/avif";
  mimeMap[".png"] = "image/png";
  mimeMap[".svg"] = "image/svg+xml";
  mimeMap[".svgz"] = "image/svg+xml";
  mimeMap[".tif"] = "image/tiff";
  mimeMap[".tiff"] = "image/tiff";
  mimeMap[".wbmp"] = "image/vnd.wap.wbmp";
  mimeMap[".webp"] = "image/webp";
  mimeMap[".ico"] = "image/x-icon";
  mimeMap[".jng"] = "image/x-jng";
  mimeMap[".bmp"] = "image/x-ms-bmp";
  mimeMap[".woff"] = "font/woff";
  mimeMap[".woff2"] = "font/woff2";
  mimeMap[".jar"] = "application/java-archive";
  mimeMap[".war"] = "application/java-archive";
  mimeMap[".ear"] = "application/java-archive";
  mimeMap[".json"] = "application/json";
  mimeMap[".hqx"] = "application/mac-binhex40";
  mimeMap[".doc"] = "application/msword";
  mimeMap[".pdf"] = "application/pdf";
  mimeMap[".ps"] = "application/postscript";
  mimeMap[".eps"] = "application/postscript";
  mimeMap[".ai"] = "application/postscript";
  mimeMap[".rtf"] = "application/rtf";
  mimeMap[".m3u8"] = "application/vnd.apple.mpegurl";
  mimeMap[".kml"] = "application/vnd.google-earth.kml+xml";
  mimeMap[".kmz"] = "application/vnd.google-earth.kmz";
  mimeMap[".xls"] = "application/vnd.ms-excel";
  mimeMap[".eot"] = "application/vnd.ms-fontobject";
  mimeMap[".ppt"] = "application/vnd.ms-powerpoint";
  mimeMap[".odg"] = "application/vnd.oasis.opendocument.graphics";
  mimeMap[".odp"] = "application/vnd.oasis.opendocument.presentation";
  mimeMap[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
  mimeMap[".odt"] = "application/vnd.oasis.opendocument.text";
  mimeMap[".pptx"] =
      "application/"
      "vnd.openxmlformats-officedocument.presentationml.presentation";
  mimeMap[".xlsx"] =
      "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  mimeMap[".docx"] =
      "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
  mimeMap[".wmlc"] = "application/vnd.wap.wmlc";
  mimeMap[".wasm"] = "application/wasm";
  mimeMap[".7z"] = "application/x-7z-compressed";
  mimeMap[".cco"] = "application/x-cocoa";
  mimeMap[".jardiff"] = "application/x-java-archive-diff";
  mimeMap[".jnlp"] = "application/x-java-jnlp-file";
  mimeMap[".run"] = "application/x-makeself";
  mimeMap[".pl"] = "application/x-perl";
  mimeMap[".pm"] = "application/x-perl";
  mimeMap[".prc"] = "application/x-pilot";
  mimeMap[".pdb"] = "application/x-pilot";
  mimeMap[".rar"] = "application/x-rar-compressed";
  mimeMap[".rpm"] = "application/x-redhat-package-manager";
  mimeMap[".sea"] = "application/x-sea";
  mimeMap[".swf"] = "application/x-shockwave-flash";
  mimeMap[".sit"] = "application/x-stuffit";
  mimeMap[".tcl"] = "application/x-tcl";
  mimeMap[".tk"] = "application/x-tcl";
  mimeMap[".der"] = "application/x-x509-ca-cert";
  mimeMap[".pem"] = "application/x-x509-ca-cert";
  mimeMap[".crt"] = "application/x-x509-ca-cert";
  mimeMap[".xpi"] = "application/x-xpinstall";
  mimeMap[".xhtml"] = "application/xhtml+xml";
  mimeMap[".xspf"] = "application/xspf+xml";
  mimeMap[".zip"] = "application/zip";
  mimeMap[".bin"] = "application/octet-stream";
  mimeMap[".exe"] = "application/octet-stream";
  mimeMap[".dll"] = "application/octet-stream";
  mimeMap[".deb"] = "application/octet-stream";
  mimeMap[".dmg"] = "application/octet-stream";
  mimeMap[".iso"] = "application/octet-stream";
  mimeMap[".img"] = "application/octet-stream";
  mimeMap[".msi"] = "application/octet-stream";
  mimeMap[".msp"] = "application/octet-stream";
  mimeMap[".msm"] = "application/octet-stream";
  mimeMap[".mid"] = "audio/midi";
  mimeMap[".midi"] = "audio/midi";
  mimeMap[".kar"] = "audio/midi";
  mimeMap[".mp3"] = "audio/mpeg";
  mimeMap[".ogg"] = "audio/ogg";
  mimeMap[".m4a"] = "audio/x-m4a";
  mimeMap[".ra"] = "audio/x-realaudio";
  mimeMap[".3gpp"] = "video/3gpp";
  mimeMap[".3gp"] = "video/3gpp";
  mimeMap[".ts"] = "video/mp2t";
  mimeMap[".mp4"] = "video/mp4";
  mimeMap[".mpeg"] = "video/mpeg";
  mimeMap[".mpg"] = "video/mpeg";
  mimeMap[".mov"] = "video/quicktime";
  mimeMap[".webm"] = "video/webm";
  mimeMap[".flv"] = "video/x-flv";
  mimeMap[".m4v"] = "video/x-m4v";
  mimeMap[".mng"] = "video/x-mng";
  mimeMap[".asx"] = "video/x-ms-asf";
  mimeMap[".asf"] = "video/x-ms-asf";
  mimeMap[".wmv"] = "video/x-ms-wmv";
  mimeMap[".avi"] = "video/x-msvideo";

  return mimeMap;
}

std::string Method::getMimeType(const std::string extension) {
  std::map<std::string, std::string> mimeMap = createMimeTypeMap();
  std::map<std::string, std::string>::iterator it = mimeMap.find(extension);
  if (it != mimeMap.end()) {
    return it->second;
  }
  return "text/html";
}

struct MultipartPart {
  std::map<std::string, std::string> headers;
  std::string content;
};

void processMultipartFile(std::fstream &file_request,
                          const std::string &boundary,
                          const std::string &outputFile) {
  std::ofstream file_out(outputFile.c_str(), std::ios::binary);
  if (!file_out.is_open()) {
    std::cerr << "Error: Could not open output file: " << outputFile
              << std::endl;
    return;
  }

  std::string delimiter = "--" + boundary;
  std::string endDelimiter = "--" + boundary + "--";
  std::string line;
  bool inContent = false;
  size_t contentStart = 0;

  while (std::getline(file_request, line)) {
    if (!line.empty() && line[line.size() - 1] == '\r') {
      line.erase(line.size() - 1); // Remove the last character
  }
  

    // If we reach the boundary, skip headers and find the content
    if (line.find(delimiter) == 0) {
      if (line == endDelimiter)
        break;           // End of multipart data
      inContent = false; // Reset when a new part starts
      continue;
    }

    // Skip headers until we reach the content
    if (!inContent) {
      if (line.empty()) {
        inContent = true; // Empty line signals the start of content
        contentStart =
            file_request.tellg(); // Mark the start position of the content
      }
      continue;
    }
  }

  file_request.seekg(contentStart, std::ios::beg);
  char buffer[4096];
  std::string lastChunk;

  while (file_request.read(buffer, sizeof(buffer))) {
    lastChunk.assign(buffer, file_request.gcount());

    // Check if endDelimiter is in this chunk
    size_t pos = lastChunk.find(endDelimiter);
    if (pos != std::string::npos) {
      file_out.write(lastChunk.c_str(), pos); // Write only up to endDelimiter
      break;
    }

    file_out.write(buffer, file_request.gcount());
  }

  // Check remaining data for endDelimiter
  lastChunk.assign(buffer, file_request.gcount());
  size_t pos = lastChunk.find(endDelimiter);
  if (pos != std::string::npos) {
    file_out.write(lastChunk.c_str(), pos); // Write only up to endDelimiter
  } else {
    file_out.write(lastChunk.c_str(), lastChunk.size());
  }

  file_out.close();
}

bool Method::allowd_method(const std::vector<std::string> &vector,
                           const std::string &method) {
  for (std::vector<std::string>::const_iterator it = vector.begin();
       it != vector.end(); ++it) {
    if (*it == method) {
      return true;
    }
  }
  return false;
}

std::string extractFileName(std::fstream &file) {
  std::string filename = "";
  std::string contentDisposition;
  file.seekg(0, std::ios::beg);
  while (std::getline(file, contentDisposition)) {
    size_t pos = contentDisposition.find("filename=");
    if (pos != std::string::npos) {
      pos += 9; // Move past "filename="

      if (pos < contentDisposition.size() && contentDisposition[pos] == '"') {
        ++pos; // Move past the opening quote
        size_t endPos = contentDisposition.find("\"", pos);
        if (endPos != std::string::npos) {
          filename = contentDisposition.substr(pos, endPos - pos);
          break; // Exit loop once filename is found
        }
      } else {
        size_t endPos = contentDisposition.find_first_of(" ;", pos);
        if (endPos != std::string::npos) {
          filename = contentDisposition.substr(pos, endPos - pos);
        } else {
          filename =
              contentDisposition.substr(pos); // Take the rest if no delimiter
        }
        break; // Exit loop once filename is found
      }
    }
  }
  file.seekg(0, std::ios::beg);
  return filename;
}

static bool fileExists(const std::string &f) {
  std::ifstream file(f.c_str());
  return (file.good());
}

bool Method::isCGIPath(const std::string &fullPath, const RouteConfig &route) {
  size_t extPos = fullPath.rfind('.');
  if (extPos != std::string::npos) {
    std::string fileExtension = fullPath.substr(extPos);
    const std::vector<std::string> &allowedExtensions = route.getCgiExtension();
    for (std::vector<std::string>::const_iterator it =
             allowedExtensions.begin();
         it != allowedExtensions.end(); ++it) {
      if (fileExtension == *it) {
        return true;
      }
    }
  }
  return false;
}

std::string Method::getFileExtension(const std::string &filePath) {
  size_t pos = filePath.rfind('.');
  if (pos != std::string::npos) {
    return filePath.substr(pos);
  }
  return "";
}

std::string Method::decodeURIComponent(const std::string &encoded) {
  std::string decoded;
  char hex[3] = {0};

  for (size_t i = 0; i < encoded.size(); ++i) {
    if (encoded[i] == '%' && i + 2 < encoded.size()) {
      hex[0] = encoded[i + 1];
      hex[1] = encoded[i + 2];
      decoded += static_cast<char>(std::strtol(hex, NULL, 16));
      i += 2;
    } else if (encoded[i] == '+') {
      decoded += ' ';
    } else {
      decoded += encoded[i];
    }
  }
  return decoded;
}

void Method::parseQueryString(const std::string &queryString,
                              std::map<std::string, std::string> &queryParams) {
  std::istringstream stream(queryString);
  std::string pair;

  while (std::getline(stream, pair, '&')) {
    size_t pos = pair.find('=');
    if (pos != std::string::npos) {
      std::string key = decodeURIComponent(pair.substr(0, pos));
      std::string value = decodeURIComponent(pair.substr(pos + 1));
      queryParams[key] = value;
    } else {
      queryParams[decodeURIComponent(pair)] = "";
    }
  }
}

bool Method::isInterpreterCompatible(const std::string &interpreter,
                                     const std::string &fileExtension) {

  typedef std::map<std::string, std::vector<std::string> > InterpreterMap;
  static InterpreterMap interpreterMap;

  if (interpreterMap.empty()) {
    std::vector<std::string> phpExtensions;
    phpExtensions.push_back(".php");
    interpreterMap["/usr/bin/php-cgi"] = phpExtensions;

    std::vector<std::string> pythonExtensions;
    pythonExtensions.push_back(".py");
    interpreterMap["/usr/bin/python3"] = pythonExtensions;
  }

  InterpreterMap::const_iterator it = interpreterMap.find(interpreter);
  if (it != interpreterMap.end()) {
    const std::vector<std::string> &supportedExtensions = it->second;

    return std::find(supportedExtensions.begin(), supportedExtensions.end(),
                     fileExtension) != supportedExtensions.end();
  }

  return false;
}

std::string trim(const std::string &line) {
  const char *WhiteSpace = " \t\v\r\n";
  std::size_t start = line.find_first_not_of(WhiteSpace);
  std::size_t end = line.find_last_not_of(WhiteSpace);
  return start == end ? std::string() : line.substr(start, end - start + 1);
}

std::map<std::string, std::string>
Method::extractQueryParams(HttpRequest &request) {
  std::map<std::string, std::string> queryParams;

  std::string queryString = request.getQueryString();
  if (!queryString.empty())
    parseQueryString(queryString, queryParams);

  if (request.getMethod() == "POST") {
    std::string contentType = trim(request.getHeader("Content-Type"));

    if (contentType == "application/x-www-form-urlencoded") {
      std::string requestBody = request.getBody1();
      for (size_t i = 0; i < requestBody.size(); i++) {
        if (requestBody[i] == '\n' || requestBody[i] == ' ')
          requestBody[i] = '&';
      }
      parseQueryString(requestBody, queryParams);
    }
  }

  return queryParams;
}

bool Method::handleCGI(HttpRequest &request, const std::string &fullPath,
                           int clientSocket, const RouteConfig &route, ServerConfig server,CGIHandler &cgiHandler) {
    try {
        const std::vector<std::string> &interpreters = route.getCgiPass();
        if (interpreters.empty()) {
            throw std::runtime_error("No CGI interpreters specified in config.");
        }

        std::map<std::string, std::string> queryParams =
            extractQueryParams(request);
        
        const std::string fileExtension = getFileExtension(fullPath);

        std::string selectedInterpreter;
        for (std::vector<std::string>::const_iterator it = interpreters.begin();
             it != interpreters.end(); ++it) {
            if (isInterpreterCompatible(*it, fileExtension)) {
                selectedInterpreter = *it;
                break;
            }
        }
        
        if (selectedInterpreter.empty()) {
            throw std::runtime_error(
                "No compatible interpreter found for the requested file.");
        }

        if(!cgiHandler.first_time)
        {
          cgiHandler.init(request, selectedInterpreter, fullPath, queryParams);
          cgiHandler.execute();
          cgiHandler.first_time = true;
        }
        if(cgiHandler.first_time)
        {
          cgiHandler.waitForCgi();
          cgiHandler.readWrite();

        }
        if(cgiHandler.cgi_is_complet)
        {
          std::stringstream response;
          response << "HTTP/1.1 " << cgiHandler.getStatusCode() << " OK\r\n";
          std::map<std::string, std::string> headers = cgiHandler.getHeaders();  
          std::map<std::string, std::string>::const_iterator header = headers.begin(); 
          for (;header != headers.end();header++) {
              response << header->first << ": " << header->second << "\r\n";
          }
          response << "\r\n";
          std::string line;
          std::fstream file_body(cgiHandler.getBodyFileName().c_str());
          if (!file_body.is_open()) {
              throw std::runtime_error("Failed to open CGI output file.");
          }
          file_body.seekg(0, std::ios::beg);

          while (std::getline(file_body, line))
          {
            response<<line<<"\n";
          }
          if(send(clientSocket, response.str().c_str(), response.str().size(), 0)<=0)
          { 
              std::cerr<<"Write failed: "<<std::endl;
          }
          cgiHandler.cleaning();
        }
        return true;
    } catch (const std::exception &e) {
        std::cerr << "CGIHandler error: " << e.what() << std::endl;
        HttpResponse::sendError(clientSocket, 500, "Internal Server Error", server);
        return false;
    }
}

void Method::HandleMethodPost(HttpRequest &request, int clientSocket,
                              const RouteConfig &route, std::string fullPath,
                              ServerConfig server) {
    std::string response;
        
   (void)server;
   (void)fullPath;
    // Handle non-CGI POST requests (e.g., file uploads)
    if (!allowd_method(route.getAllowedMethods(), "POST")) {
        response = buildResponse(405, "Method Not Allowed", "POST method is not allowed.");
        if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
        return;
    }

    if (request.getBody().tellg() == 0) {
        response = buildResponse(400, "Bad Request", "Request body is empty.");
        if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
        return;
    }

    std::string file_name = extractFileName(request.getBody());
    if (file_name.empty()) {
        response = buildResponse(400, "Bad Request", "Filename is missing in the request.");
        if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
        return;
    }

    std::string static_path = route.getRoot();
    std::string file_path = "./www/post/upload/" + file_name;

    if (fileExists(file_path)) {
        response = buildResponse(409, "Conflict", "File already exists.");
        if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
        return;
    }

    std::ofstream file(file_path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        response = buildResponse(500, "Internal Server Error", "Unable to create or write to file.");
        if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
        return;
    }

    if (request.getMultiformFlag()) {
        std::fstream &file_request(request.file_body);
        std::string boundary = request.getBoundary();
        boundary.erase(boundary.find_last_not_of(" \t\r\n") + 1);
        processMultipartFile(file_request, boundary, file_path);
    } else {
        file.write(request.getBody1().c_str(), request.getBody1().length());
    }

    file.close();
    response = buildResponse(201, "Created", "File upload successful.");
    if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
    { 
        std::cerr<<"Write failed: "<<std::endl;
    }
}

std::string Method::buildResponse(int statusCode,
                                  const std::string &statusMessage,
                                  const std::string &body) {
  std::stringstream response;
  std::string css =
      "<style>\nbody {\nfont-family: Arial, sans-serif;\nbackground-color: "
      "#f4f4f4;\ncolor: #333;\nmargin: 0;\npadding: 0;\ndisplay: "
      "flex;\njustify-content: center;\nalign-items: center;\nheight: "
      "100vh;\nflex-direction: column;\n}\nh1 {\ncolor: #4CAF50;\nfont-size: "
      "2.5em;\n}\np {\nfont-size: 1.2em;\nmargin-top: 10px;\n}\n</style>";
  std::string htmlBody =
      "<!DOCTYPE html>\n<html>\n<head>\n<title>Server Response</title>" + css +
      "\n</head>\n<body>\n<h1>" + statusMessage + "</h1>\n<p>" + body +
      "</p>\n</body>\n</html>";
  response << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n"
           << "Content-Type: text/html\r\n"
           << "Content-Length: " << htmlBody.size() << "\r\n"
           << "Connection: close\r\n\r\n"
           << htmlBody;
  return response.str();
}