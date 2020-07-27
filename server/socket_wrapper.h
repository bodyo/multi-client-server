#pragma once

#include <optional>
#include <string>

/**
 * \brief SocketWrapper class wraps C socket layer and provide simple access for
 * socket functionality.
 * \note SocketWrapper::init() function should be called only for socket that
 * will accept connections.
 */
class SocketWrapper {
public:
  SocketWrapper() = default;
  SocketWrapper(int fd);

  bool init(const std::string &ip_addr, int port);
  bool send(std::string data);
  std::optional<SocketWrapper> accept();
  std::optional<std::string> recv();

  int fd() const;

private:
  int fd_;
};
