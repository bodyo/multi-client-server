#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

void handleInput(int client_fd) {
  while (true) {
    char data[1024];
    int read = recv(client_fd, data, 1024, 0);
    if (read <= 0) {
      std::cout << "[-]Error in receiving data.\n";
      close(client_fd);
      exit(1);
    }
    data[read] = '\0';
    printf("%c[2K\r", 27);
    std::cout << data << std::endl;
    std::cout << ">: ";
    std::cout.flush();
  }
}

int main(int argc, char **args) {
  if (argc < 2) {
    std::cerr << "Missed command line args.";
    exit(1);
  }

  int portNum = atoi(args[2]);
  int bufsize = 1024;
  char buffer[bufsize];

  struct sockaddr_in server_addr;
  int client = socket(AF_INET, SOCK_STREAM, 0);

  if (client < 0) {
    std::cout << "Error createing socket." << std::endl;
    exit(1);
  }

  std::cout << "Client Socket created." << std::endl;
  server_addr.sin_family = AF_INET;

  server_addr.sin_addr.s_addr = inet_addr(args[1]);
  server_addr.sin_port = htons(portNum);
  if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      0)
    std::cout << "=> Connection to the server "
              << inet_ntoa(server_addr.sin_addr)
              << " with port number: " << portNum << std::endl;
  else {
    std::cerr << "Unable to connect to the server with following address: "
              << args[1] << ':' << portNum << std::endl;
    exit(1);
  }
  std::cout << "Connection confirmed." << std::endl;
  std::cout << "Enter 'exit()' to end the connection." << std::endl;
  recv(client, buffer, bufsize, 0);
  std::cout << '\n' << buffer << '\n';

  std::thread handleIngressData(handleInput, client);

  std::string input;
  while (true) {
    std::cout << ">: ";
    std::cout.flush();
    std::getline(std::cin, input);
    if (input == ":exit") {
      std::cout << "[-]Disconect from the server\n";
      std::cout.flush();
      close(client);
      exit(0);
    }
    input.push_back('\0');
    if (send(client, input.c_str(), input.size(), 0) <= 0) {
      break;
    }
  }
  handleIngressData.join();
  std::cout << "Connection terminated." << std::endl;
  close(client);
  return 0;
}
