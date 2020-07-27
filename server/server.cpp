#include "server.h"

#include <algorithm>
#include <iostream>
#include <string>

Server::Server(const std::string &ip_addr, int port)
    : m_ip_addr(ip_addr), m_port(port) {}

Server::~Server() {
  std::for_each(std::begin(thread_pool), std::end(thread_pool),
                [](std::thread &th) {
                  if (th.joinable())
                    th.join();
                });
}

bool Server::init() {
  std::cout << "Server listens on " << m_ip_addr << ':' << m_port << '.'
            << std::endl;
  return m_socket_FD.init(m_ip_addr, m_port);
}

/*!
 * \brief Server::exec() method runs in main thread and handle new clients
 * \note after new client is accepted the new thread(Server::clientHandler) for
 * handle this client will be created
 */
void Server::exec() {
  std::cout << "exec started\n";

  const std::string hello(
      "Hello from ring test server.\nPlease enter your nickname!\0");

  while (true) {
    auto c_fd = m_socket_FD.accept();
    if (!c_fd) {
      std::cerr << "accept failed";
      continue;
    }
    auto client_fd = *c_fd;

    if (std::find_if(std::cbegin(clients_db), std::cend(clients_db),
                     [&client_fd](const std::shared_ptr<Client> &client) {
                       return client->socket.fd() == client_fd.fd();
                     }) == std::cend(clients_db)) {
      std::cout << "New client attempt to connect\n";
      if (!client_fd.send(hello)) {
        continue;
      }
      auto client = std::make_shared<Client>(Client{"", client_fd});
      client_db_mux.lock();
      clients_db.push_back(client);
      client_db_mux.unlock();

      std::weak_ptr<Client> wp_client = client;
      thread_pool.push_back(
          std::thread(&Server::clientHandler, this, wp_client));
    }
  }
}

/*!
 * \brief Server::clientHandler method runs in separate thread and handle
 * client's messges
 * \note after new client is accepted the new thread(Server::clientHandler) for
 * handle this client will be created
 */
void Server::clientHandler(const std::weak_ptr<Client> &weak_ptr_client) {
  // get client and as this is begining of new thread read previously asked
  // client nickname.
  if (auto client = weak_ptr_client.lock()) {
    if (client->client_name.empty()) {
      auto resp = client->socket.recv();
      if (!resp) {
        std::cerr << "Clinet disconnected" << std::endl;
        std::cout.flush();
        return;
      }
      client->client_name = *resp;
    }
  }

  // loop where client's messages handled
  while (true) {
    auto client = weak_ptr_client.lock();
    if (!client) {
      return;
    }
    auto repl = client->socket.recv();
    if (!repl) {
      std::cerr << "Client disconnected. " << client->client_name << std::endl;
      std::cout.flush();
      const std::lock_guard<std::mutex> lock(client_db_mux);
      clients_db.remove(client);
      return;
    }

    const std::lock_guard<std::mutex> lock(client_db_mux);
    for (const auto &send_to : clients_db) {
      const bool is_ourselves = send_to->socket.fd() == client->socket.fd();
      // check is client that we want to sent is valid and it is not ourselves
      if (send_to && !is_ourselves) {
        std::string message('[' + client->client_name + ']' + ": ");
        message.append(*repl);

        if (!send_to->socket.send(message)) {
          std::cerr << "Client disconnected. " << send_to->client_name
                    << std::endl;
          std::cout.flush();
          clients_db.remove(send_to);
          return;
        }
      }
    }
  }
}
