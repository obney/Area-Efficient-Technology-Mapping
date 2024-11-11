CXX = g++
CXXFLAGS = -std=c++17 -O3 -g
TARGET = mapper
SRC = optmap.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) 

clean:
	rm -f $(TARGET) $(TARGET).o
