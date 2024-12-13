# Compilers
CC = gcc
CXX = g++

# Flags
CFLAGS = -Wall -g -I./headers -I./source -I/usr/include/libcamera \
		 -I/usr/include/opencv4 -D USE_BCM2835_LIB -O2
CXXFLAGS = -Wall -g -I/usr/include/libcamera -I/usr/include/opencv4 \
		   -I./libraries -std=c++17 -O2

# Libraries
LIBS = -lstdc++ -lcamera -lcamera-base -lopencv_core -lopencv_imgcodecs \
       -lopencv_imgproc -lopencv_highgui -lbcm2835 -lm -lsl_lidar_sdk
LIDAR_LIB = -L./libraries

# Directories
OUTDIR = out

# Source Files
CFILES := $(wildcard source/*.c)
CPPFILES := $(wildcard source/*.cpp)

# Object Files
COBJECTS := $(patsubst source/%.c,$(OUTDIR)/%.o,$(wildcard source/*.c))

CPPOBJECTS := $(patsubst source/%.cpp,$(OUTDIR)/%.o,$(wildcard source/*.cpp))

# Target executable
TARGET = waymore

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(COBJECTS) $(CPPOBJECTS)
	$(CXX) $(COBJECTS) $(CPPOBJECTS) -o $(TARGET) $(LIBS) $(LIDAR_LIB)

# Compile C++ files
$(OUTDIR)/%.o: source/%.cpp
	@mkdir -p $(OUTDIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Compile C files
$(OUTDIR)/%.o: source/%.c
	@mkdir -p $(OUTDIR)
	$(CC) -c $< -o $@ $(CFLAGS)

# Run target
run: $(TARGET)
	sudo ./$(TARGET)

# Clean target
clean:
	rm -rf $(TARGET) $(OUTDIR)

.PHONY: all clean