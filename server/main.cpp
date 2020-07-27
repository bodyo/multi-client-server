#include "server.h"

#include <iostream>
#include <stdlib.h>
#include <string>

int extractPortNumberFromArgs(int argc, char **args) {
  if (argc < 2) {
    std::cerr << "Invalid number of arguments.";
    exit(1);
  }

  return atoi(args[1]);
}

int main(int argc, char **args) {
  if (argc < 3) {
    std::cerr << "Invalid number of arguments.";
    exit(1);
  }

  Server server(std::string(args[1]), atoi(args[2]));
  if (server.init()) {
    server.exec();
  }
}
