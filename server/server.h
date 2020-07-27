#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include <memory>
#include <mutex>
#include <thread>

#include "socket_wrapper.h"

/**
 * \brief Client struct represents client with client_name(nickname) and socket
 * that belongs to this name.
 */
struct Client {
  std::string client_name;
  SocketWrapper socket;
};

/**
 * \brief Server class waiting for clients and handle their comunication.
 * We create new thread for each new client(it's not the best solution. I think
 * that it's possible to do it more clever way)
 */
class Server {
public:
  Server() = delete;
  // non copyable
  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;

  Server(const std::string &ip_addr, int port);
  ~Server();

  bool init();
  void exec();

private:
  void clientHandler(const std::weak_ptr<Client> &weak_ptr_client);

private:
  std::string m_ip_addr;     // < server's IP address
  int m_port;                // < server's port
  SocketWrapper m_socket_FD; // < server's socket

  std::list<std::shared_ptr<Client>> clients_db;
  // mutext for clients_db containter protecting
  std::mutex client_db_mux;
  // bug the thread is not removed after disconnecting the client. If we will
  // have many disconnected clients this containter will contain many dummy
  // threads that do nothing.
  std::vector<std::thread> thread_pool;
};
