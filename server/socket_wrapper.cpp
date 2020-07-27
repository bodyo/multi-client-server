#include "socket_wrapper.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

SocketWrapper::SocketWrapper(int fd) : fd_(fd) {}

/**
 * \brief SocketWrapper::init method initializes socket and bind it to network
 * interface.
 * \note Should be used only with sockets that accept connections.
 */
bool SocketWrapper::init(const std::string &ip_addr, int port) {
  sockaddr_in sa;
  fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd_ == -1) {
    std::cerr << "cannot create socket";
    return false;
  }

  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  inet_pton(AF_INET, ip_addr.c_str(), &sa.sin_addr);

  if (bind(fd_, (struct sockaddr *)&sa, sizeof sa) == -1) {
    std::cerr << "bind failed";
    close(fd_);
    return false;
  }

  if (listen(fd_, 10) == -1) {
    perror("listen failed");
    close(fd_);
    return false;
  }

  return true;
}

/**
 * \brief SocketWrapper::send method sends data to socket.
 * \return true if data sended otherway returns false.
 */
bool SocketWrapper::send(std::string data) {
  // add null terminator cause std::string::c_str() return string without it.
  data.push_back('\0');
  if (::send(fd_, data.c_str(), data.size() + 1, 0) <= 0) {
    return false;
  }
  return true;
}

/**
 * \brief SocketWrapper::recv method reads data from socket.
 * \return data from socket if success otherway returns std::nullopt
 */
std::optional<std::string> SocketWrapper::recv() {
  constexpr int size = 1024;
  char data[size];
  // BUG: if data is bigger than 1024 we will read not all data.
  // should be updated
  if (::recv(fd_, data, size, 0) <= 0) {
    return {};
  }

  return data;
}

/**
 * \brief SocketWrapper::accept accepts new clients.
 * \return valid SocketWrapper success otherway returns std::nullopt.
 */
std::optional<SocketWrapper> SocketWrapper::accept() {
  int client_fd = ::accept(fd_, NULL, NULL);
  std::cout << "accept\n";
  if (client_fd < 0) {
    std::cerr << "accept failed";
    return {};
  }
  return {client_fd};
}

int SocketWrapper::fd() const { return fd_; }
