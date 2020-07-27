  CXX = g++

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CXXFLAGS  = -g -Wall -std=c++17 -pthread

  # the build target executable:
  TARGET = client

  client: main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp
