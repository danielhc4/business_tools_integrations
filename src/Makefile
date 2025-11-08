CXX = g++
CXXFLAGS = -O2 -std=c++17
LDLIBS = -lfcgi -lfcgi++


all: fcgi-app


fcgi-app: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o fcgi-app $(LDLIBS)


clean:
	rm -rf fcgi-app


.PHONY: all clean