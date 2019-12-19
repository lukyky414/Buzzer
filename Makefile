CXX = g++ -pthread -lxdo
CXXFLAGS = -g -Wall -O2
LDFLAGS = `pkg-config --cflags --libs libusb-1.0`


TARGET = main
SOURCE = $(wildcard *.cpp) $(wildcard *.c)
HEADERS = $(wildcard *.hpp) $(wildcard *.h)

TARGET: $(HEADERS)
	$(CXX) -o $(TARGET) $(SOURCE) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f *.o *~ $(TARGET)
