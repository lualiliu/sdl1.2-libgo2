CC = /opt/rg351p-toolchain/bin/aarch64-buildroot-linux-gnu-gcc
CXX = /opt/rg351p-toolchain/bin/aarch64-buildroot-linux-gnu-g++
STRIP = /opt/rg351p-toolchain/bin/aarch64-buildroot-linux-gnu-strip

CFLAGS = -O0 
CXXFLAGS = $(CFLAGS)
LDFLAGS =  -lc -lgcc -lm -Wl,--as-needed -Wl,--gc-sections -ldrm -lSDL -s -flto -lopenal -lEGL -levdev -lgbm -lpthread -ldrm -lm -lrga -lpng -lasound

OBJDIR = ./src
APPNAME = main

SOURCES := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp, src/%.o, $(SOURCES))

# File types rules
$(OBJDIR)/src/%.o: src/%.cpp src/%.h
	$(CXX) $(CFLAGS) -o $@ -c $<

all: shared

debug: $(OBJS)
	$(CC) -o $(APPNAME)-debug $(OBJS) $(LDFLAGS)

shared: debug
	$(STRIP) $(APPNAME)-debug -o $(APPNAME)


