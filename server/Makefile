  CXX = g++

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  #  -std=c++17 enables c++ 17 standart
  #  -pthread links posix threads (used by std::thread)
  CXXFLAGS  = -g -Wall -std=c++17 -pthread

  # the build target executable:
  TARGET = server

  server: main.cpp server.cpp server.h socket_wrapper.h socket_wrapper.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) server.cpp main.cpp socket_wrapper.cpp
