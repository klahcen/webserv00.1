#include "../include/HttpRequest.hpp"
#include "../include/ServerConfig.hpp"
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <sys/socket.h>

HttpRequest::HttpRequest() {
  method = "";
  path = "";
  version = "";
  nextchunk = "";
  fileName = "";
  type = "";
  length = 0;
  buffer = 0;
  headersComplete = false;
  contentLength = 0;
  isChunked = false;
  chack = false;
  bodyBuffer = 0;
  ftype = NULL;
  path_file = "";
  totalesize = 0;
  checkfirsttime = true;
  requestcomplet = false;
  bytesRead = 0;
}

void HttpRequest::generateRandomFileName() {

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
}

HttpRequest::HttpRequest(const HttpRequest &other)
    : method(other.method), path(other.path), version(other.version),
      headers(other.headers), nextchunk(other.nextchunk),
      fileName(other.fileName), type(other.type), length(other.length),
      buffer(other.buffer), headersComplete(other.headersComplete),
      contentLength(other.contentLength), isChunked(other.isChunked),
      chack(other.chack), body(other.body), totalesize(other.totalesize),
      requestcomplet(other.requestcomplet) {

  // Deep copy bodyBuffer if it exists
  this->path_file = other.path_file;
  this->bytesRead = other.bytesRead;
  this->checkfirsttime = other.checkfirsttime;
  if (other.bodyBuffer) {
    bodyBuffer = new char[strlen(other.bodyBuffer) + 1];
    strcpy(bodyBuffer, other.bodyBuffer);
  } else {
    bodyBuffer = NULL;
  }

  // Handle file pointer (ftype) - it cannot be copied
  ftype = NULL; // Avoid copying FILE* directly

  // Handle fstream (file_body) - cannot be copied, must be reopened
  if (!other.path_file.empty()) {
    file_body.open(other.path_file.c_str(), std::ios::binary);
  }
}

// **Copy Assignment Operator**
HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
  if (this == &other)
    return *this; // Self-assignment check

  // Copy primitive and STL data members
  method = other.method;
  path = other.path;
  version = other.version;
  headers = other.headers;
  nextchunk = other.nextchunk;
  fileName = other.fileName;
  type = other.type;
  length = other.length;
  buffer = other.buffer;
  headersComplete = other.headersComplete;
  contentLength = other.contentLength;
  isChunked = other.isChunked;
  chack = other.chack;
  body = other.body;
  path_file = other.path_file;
  totalesize = other.totalesize;
  requestcomplet = other.requestcomplet;
  bytesRead = other.bytesRead;
  checkfirsttime = other.checkfirsttime;

  // Deep copy bodyBuffer
  delete[] bodyBuffer;
  if (other.bodyBuffer) {
    bodyBuffer = new char[strlen(other.bodyBuffer) + 1];
    strcpy(bodyBuffer, other.bodyBuffer);
  } else {
    bodyBuffer = NULL;
  }

  // Handle file pointer (ftype) - it cannot be copied
  if (ftype) {
    fclose(ftype);
    ftype = NULL;
  }

  // Handle fstream (file_body) - must be reopened
  file_body.close();
  if (!other.path_file.empty()) {
    file_body.open(other.path_file.c_str(),
                   std::ios::in | std::ios::out | std::ios::binary);
  }

  return *this;
}

size_t HttpRequest::getcontentLength() { return contentLength; }

HttpRequest::~HttpRequest() {
}

std::string HttpRequest::get_pathfile() { return path_file; }

bool HttpRequest::parseRequestLine(const std::string &requestLine) {
  if(std::isspace(requestLine[0]))
    return false;
  std::istringstream lineStream(requestLine);
  if (!(lineStream >> method >> path >> version)) {
    std::cerr << "Invalid request line: " << requestLine << std::endl;
    return (false);
  }
  if(version!="HTTP/1.1"&&version!="HTTP/1.0")
    return false;
  return (true);
}

std::string toupper(const std::string& str) {
    std::string result = str; // Create a copy of the input string

    // Iterate over each character using a traditional for loop
    for (std::string::size_type i = 0; i < result.size(); ++i) {
        result[i] = std::toupper(result[i]); // Convert to uppercase
    }

    return result; // Return the modified string
}

bool HttpRequest::parseHeaders(std::istringstream &requestStream) {
  std::string header;
  while (std::getline(requestStream, header) && header != "\r") {
    std::string::size_type delimiterPos = header.find(':');
    if (delimiterPos != std::string::npos) {
      std::string key = header.substr(0, delimiterPos);
      if(key.empty()||key.find(' ')!=std::string::npos)
        return false;
      std::string value = header.substr(delimiterPos + 1);
      size_t start = 0;
      for(; start<value.size(); start++)
      {
        if(!std::isspace(value[start]))
          break;
      }
      if (start)
          value.erase(0, start);
      if(value.empty())
        return false;
      headers[key] = value;
    } else {
      std::cerr << "Malformed header: " << header << std::endl;
      return (false);
    }
  }
  std::map<std::string, std::string>::iterator head = headers.begin();
  for(;head!=headers.end();head++)
  {
    if(toupper(head->first)=="HOST")
      return true;
  }
  if(head == headers.end())
    return false;
  return (true);
}

void HttpRequest::postChunked(const std::string &socketStream) {
  size_t position = 0;
  std::stringstream bodyStream;

  while (position < socketStream.size()) {
    // Find the position of the next CRLF to extract the chunk size
    size_t crlfPos = socketStream.find("\r\n", position);
    if (crlfPos == std::string::npos) {
      std::cerr << "Malformed chunk: missing CRLF after chunk size"
                << std::endl;
      break;
    }

    // Extract the chunk size as a hexadecimal string
    std::string chunkSizeHex =
        socketStream.substr(position, crlfPos - position);
    position = crlfPos + 2; // Move position past the CRLF

    // Convert the chunk size from hexadecimal to an integer
    size_t chunkSize = 0;
    try {
      chunkSize = std::strtoul(chunkSizeHex.c_str(), NULL, 0);
    } catch (const std::exception &e) {
      std::cerr << "Invalid chunk size: " << chunkSizeHex << std::endl;
      break;
    }

    // A chunk size of 0 indicates the end of the chunks
    if (chunkSize == 0) {
      break;
    }

    // Ensure there's enough data for the chunk
    if (position + chunkSize > socketStream.size()) {
      std::cerr << "Malformed chunk: not enough data for chunk size"
                << std::endl;
      break;
    }

    // Extract the chunk data
    bodyStream.write(socketStream.data() + position, chunkSize);
    position += chunkSize;

    // Ensure there's a CRLF after the chunk data
    if (socketStream.substr(position, 2) != "\r\n") {
      std::cerr << "Malformed chunk: missing CRLF after chunk data"
                << std::endl;
      break;
    }
    position += 2; // Move position past the CRLF
  }

  // Store the accumulated body data

  if (file_body.is_open())
    file_body.write(bodyStream.str().c_str(), bodyStream.str().size());
}

void HttpRequest::postBinary(const std::string &req)
{
  if (!headersComplete) {
    std::cerr << "Headers must be parsed before reading the binary body."
              << std::endl;
    return;
  }

  std::map<std::string, std::string>::iterator contentLengthIt =
      headers.find("Content-Length");
  if (contentLengthIt == headers.end()) {
    std::cerr << "Content-Length header is missing for binary data."
              << std::endl;
    return;
  }

  size_t contentLength = 0;
  try {
    contentLength = std::strtoul(contentLengthIt->second.c_str(), NULL, 0);
  } catch (const std::exception &e) {
    std::cerr << "Invalid Content-Length value: " << contentLengthIt->second
              << std::endl;
    return;
  }
  if (file_body.is_open()) {
    file_body.write(req.substr(0, contentLength).c_str(),
                    req.substr(0, contentLength).size());
  }
}


bool HttpRequest::readFromSocket(std::string &persistentBuffer) {
  bytesRead += persistentBuffer.size();
  if (checkfirsttime) {
    if (!file_body.is_open()) {
      generateRandomFileName();
      totalesize = 0;
      if (!path_file.empty() && !file_body.is_open()) {
        file_body.open(path_file.c_str(),
                       std::ios::in | std::ios::out | std::ios::app);
        if (!file_body) {
          std::cerr << "Failed to open file in assignment operator: "
                    << path_file << std::endl;
        }
      }
    }
    checkfirsttime = false;
  }
  if (!headersComplete) {
    if (!body.empty())
      persistentBuffer = body + persistentBuffer;
    size_t headerEnd = persistentBuffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
    {
        headerEnd = persistentBuffer.find("\r\n");
        if (headerEnd == std::string::npos)
        {
          requestcomplet = true;
          return false;
        }
    }
    std::string headersAndRequest = persistentBuffer.substr(0, headerEnd + 2);
    std::istringstream requestStream(headersAndRequest);
    std::string requestLine;
    if (!std::getline(requestStream, requestLine) || !parseRequestLine(requestLine))
    {
      std::cerr << "Failed to parse request line" << std::endl;
      requestcomplet = true;
      return false;
    }
    if (!parseHeaders(requestStream)) {
      std::cerr << "Failed to parse headers" << std::endl;
      requestcomplet = true;
      return false;
    }
    persistentBuffer.erase(0, headerEnd + 4);
    headersComplete = true;

    if (headers.find("Content-Length") != headers.end())
    {
      contentLength = std::strtoul(headers["Content-Length"].c_str(), NULL, 0);
      ServerConfig sl;
      size_t sizeMaxofbody = sl.getClientMaxBodySize();
      if (contentLength > sizeMaxofbody)
        return false;
    } else
    {
      requestcomplet = true;
    }
    body.append(persistentBuffer.c_str(), persistentBuffer.size());
    if (file_body.is_open())
      file_body.write(persistentBuffer.c_str(), persistentBuffer.size());
  }
  if (method == "POST") {
    if (body.size() >= (size_t)contentLength) {
      requestcomplet = true;
      return true;
    }
    if (headers.find("Transfer-Encoding") != headers.end())
      postChunked(persistentBuffer);
    else
      postBinary(persistentBuffer);
    totalesize += persistentBuffer.size();
    file_body.flush();
  }
  if (totalesize >= (ssize_t)contentLength) {
    // file_body.flush();
    requestcomplet = true;
  }
  persistentBuffer.clear();
  return true;
}

std::string HttpRequest::getMethod() const { return (method); }

std::string HttpRequest::getPath() const { return (path); }

std::string HttpRequest::getVersion() const { return (version); }

std::string HttpRequest::getHeader(const std::string &headerName) const {
  std::map<std::string, std::string>::const_iterator it =
      headers.find(headerName);
  if (it != headers.end())
    return (it->second);
  return "";
}
std::fstream &HttpRequest::getBody() { return (file_body); }

std::string HttpRequest::getBody1() {
  std::stringstream os;
  std::string line;

  file_body.clear();  // Clear any error flags
  file_body.seekg(0, std::ios::beg); // Ensure reading from the beginning

  while (std::getline(file_body, line)) {
      os << line << '\n';  // Preserve line breaks
  }

  file_body.clear();  // Clear flags again in case EOF was set
  file_body.seekg(0, std::ios::beg); // Reset for future reads

  return os.str();
}

int HttpRequest::getContentLength() const {
  std::map<std::string, std::string>::const_iterator it =
      headers.find("Content-Length");
  if (it != headers.end())
    return atoi(it->second.c_str());
  return 0; // Default to 0 if no Content-Length header
}

std::string HttpRequest::getContentType() const {
  std::map<std::string, std::string>::const_iterator it =
      headers.find("Content-Type");
  if (it != headers.end())
    return it->second;
  return ""; // Default to empty string if no Content-Type header
}


std::string HttpRequest::getBoundary() {
  std::string contentType = getHeader("Content-Type");
  size_t pos = contentType.find("boundary=");
  if (pos != std::string::npos) {
    size_t start = pos + 9; // Length of "boundary="
    size_t end =
        contentType.find(';', start); // Look for semicolon after the boundary
    if (end == std::string::npos)
      end = contentType.size(); // No semicolon, use end of string
    return contentType.substr(start, end - start);
  }
  return ""; // Boundary not found
}

bool HttpRequest::getMultiformFlag() {
  if (getBoundary() != "")
    return true;
  return false;
}

std::string HttpRequest::getQueryString() const {
  // Assuming `path` is a member variable storing the full URL path
  // Example: "/script.cgi?key1=value1&key2=value2"
  size_t queryStart = path.find('?');
  if (queryStart != std::string::npos) {
    return path.substr(queryStart + 1); // Return everything after '?'
  }
  return ""; // No query string present
}