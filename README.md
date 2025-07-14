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
### 4. Run the server with a configuration file
```bach
./webserv configs/multiserver.conf
```
