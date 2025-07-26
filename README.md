# Webserv

**"This is when you finally understand why URLs start with HTTP."**

## 👨‍💻 Contributors

This project was developed by:

- **Lahcen Kazaz**  
- **Amine Kaddouri**



## 📝 Overview

**Webserv** is a fully functional HTTP/1.0 web server implemented from scratch in **C++98** without using external libraries. This project was developed as part of the 1337 School curriculum and aims to provide a deep understanding of the HTTP protocol and low-level network programming.

It supports multiple features such as:
- Handling of **GET**, **POST**, and **DELETE** HTTP methods.
- Support for **CGI** execution (e.g., PHP/Python).
- **File upload**, **directory listing**, **error pages**, and **configuration-based routing**.
- **Multiple ports** and **non-blocking I/O** using `epoll()`.

## ⚙️ Features

- C++98 compatible, written from scratch.
- Non-blocking socket I/O using `epoll()`.
- Static file serving (HTML, CSS, JS, etc.).
- Configurable via custom `.conf` files (inspired by NGINX).
- CGI execution (for dynamic content).
- File upload handling.
- Accurate HTTP response status codes.
- Compatible with standard web browsers.

## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone git@github.com:klahcen/webserv00.1.git
```
### 2. Navigate to the project directory
```bach
cd webserv00.1
```
### 3. Compile the project
```bach
make
```

### 2. Diagram of file Webserv
```js
graph TD
    Client["Client (Browser/HTTP Client)"]:::network
    Entrypoint["main.cpp"]:::config
    ConfigParser["ConfigParser"]:::config
    ServerConfig["ServerConfig"]:::config
    RouteConfig["RouteConfig"]:::config
    Server["Server (epoll Reactor)"]:::network
    ClientObj["Client Connection Object"]:::network
    HttpRequest["HttpRequest Parser"]:::parser
    Router["Dispatcher / Router"]:::parser
    Static["Static File Handler"]:::handler
    DirList["Directory Listing Handler"]:::handler
    CGIHandler["CGIHandler"]:::handler
    GET["MethodGet"]:::handler
    POST["MethodPost"]:::handler
    DELETE["MethodDelete"]:::handler
    HttpResponse["HttpResponse Builder"]:::parser
    FS["Filesystem (page_web/ & www/)"]:::external
    CGIScripts["CGI Scripts (www/cgi/cgi-bin/)"]:::external

    Client -->|"connect"| Server
    Entrypoint -->|"starts"| ConfigParser
    ConfigParser -->|"builds"| ServerConfig
    ConfigParser -->|"builds"| RouteConfig
    Entrypoint -->|"initializes"| Server
    Server -->|"epoll_wait"| ClientObj
    ClientObj -->|"read bytes"| HttpRequest
    HttpRequest -->|"parsed request"| Router
    Router -->|"GET"| GET
    Router -->|"POST"| POST
    Router -->|"DELETE"| DELETE
    Router -->|"Static/DIR"| Static
    Router -->|"List DIR"| DirList
    Router -->|"CGI"| CGIHandler
    CGIHandler -->|"fork/exec"| CGIScripts
    GET -->|"reads"| FS
    Static -->|"reads"| FS
    DirList -->|"reads"| FS
    POST -->|"writes"| FS
    DELETE -->|"removes"| FS
    GET -->|"generates"| HttpResponse
    POST -->|"generates"| HttpResponse
    DELETE -->|"generates"| HttpResponse
    Static -->|"generates"| HttpResponse
    DirList -->|"generates"| HttpResponse
    CGIHandler -->|"generates"| HttpResponse
    HttpResponse -->|"send via socket"| ClientObj
    ClientObj -->|"write"| Client

    click Entrypoint "https://github.com/klahcen/webserv00.1/blob/main/main.cpp"
    click ConfigParser "https://github.com/klahcen/webserv00.1/blob/main/include/ConfigParser.hpp"
    click ConfigParser "https://github.com/klahcen/webserv00.1/blob/main/src/ConfigParser.cpp"
    click ServerConfig "https://github.com/klahcen/webserv00.1/blob/main/include/ServerConfig.hpp"
    click ServerConfig "https://github.com/klahcen/webserv00.1/blob/main/src/ServerConfig.cpp"
    click RouteConfig "https://github.com/klahcen/webserv00.1/blob/main/include/RouteConfig.hpp"
    click RouteConfig "https://github.com/klahcen/webserv00.1/blob/main/src/RouteConfig.cpp"
    click Server "https://github.com/klahcen/webserv00.1/blob/main/include/Server.hpp"
    click Server "https://github.com/klahcen/webserv00.1/blob/main/src/Server.cpp"
    click ClientObj "https://github.com/klahcen/webserv00.1/blob/main/include/Client.hpp"
    click HttpRequest "https://github.com/klahcen/webserv00.1/blob/main/include/HttpRequest.hpp"
    click HttpRequest "https://github.com/klahcen/webserv00.1/blob/main/src/HttpRequest.cpp"
    click HttpResponse "https://github.com/klahcen/webserv00.1/blob/main/include/HttpResponse.hpp"
    click HttpResponse "https://github.com/klahcen/webserv00.1/blob/main/src/HttpResponse.cpp"
    click GET "https://github.com/klahcen/webserv00.1/blob/main/src/MethodGet.cpp"
    click POST "https://github.com/klahcen/webserv00.1/blob/main/src/MethodPost.cpp"
    click DELETE "https://github.com/klahcen/webserv00.1/blob/main/src/MethodDelete.cpp"
    click CGIHandler "https://github.com/klahcen/webserv00.1/blob/main/include/CGIHandler.hpp"
    click CGIHandler "https://github.com/klahcen/webserv00.1/blob/main/src/CGIHandler.cpp"
    click CGIScripts "https://github.com/klahcen/webserv00.1/tree/main/www/cgi/cgi-bin/"

    classDef network fill:#f9f,stroke:#333,stroke-width:1px;
    classDef config fill:#bbf,stroke:#333,stroke-width:1px;
    classDef parser fill:#bfb,stroke:#333,stroke-width:1px;
    classDef handler fill:#ffb,stroke:#333,stroke-width:1px;
    classDef external fill:#ddd,stroke:#333,stroke-width:1px;
```
### 4. Run the server with a configuration file
```bach
./webserv configs/multiserver.conf
```
