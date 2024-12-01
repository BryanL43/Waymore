# Compilers
CC = gcc
CXX = g++

# Flags
CFLAGS = -Wall -g -I./headers -I/usr/include/libcamera \
		 -I/usr/include/opencv4 -D USE_BCM2835_LIB
CXXFLAGS = -Wall -g -I/usr/include/opencv4 -I/usr/include/libcamera -std=c++17

# Libraries
LIBS = -lstdc++ -lcamera -lcamera-base -lopencv_core -lopencv_imgcodecs \
       -lopencv_imgproc -lopencv_highgui -lbcm2835 -lm

# Directories
OUTDIR = out
SRCDIRS = source libraries

# Source Files
CFILES := $(wildcard source/*.c) $(wildcard libraries/*.c)
CPPFILES := $(wildcard source/*.cpp) $(wildcard libraries/*.cpp)

# Object Files
COBJECTS := $(patsubst source/%.c,$(OUTDIR)/%.o,$(wildcard source/*.c)) \
            $(patsubst libraries/%.c,$(OUTDIR)/%.o,$(wildcard libraries/*.c))

CPPOBJECTS := $(patsubst source/%.cpp,$(OUTDIR)/%.o,$(wildcard source/*.cpp)) \
              $(patsubst libraries/%.cpp,$(OUTDIR)/%.o,$(wildcard libraries/*.cpp))

# Target executable
TARGET = waymore

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(COBJECTS) $(CPPOBJECTS)
	$(CXX) $(COBJECTS) $(CPPOBJECTS) -o $(TARGET) $(LIBS)

# Compile C++ files
$(OUTDIR)/%.o: source/%.cpp
	@mkdir -p $(OUTDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C files
$(OUTDIR)/%.o: source/%.c
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%.o: libraries/%.c
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run target
run:
	sudo ./$(TARGET)

# Clean target
clean:
	rm -rf $(TARGET) $(OUTDIR)

.PHONY: all clean
