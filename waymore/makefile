# Compilers
CC = gcc
CXX = g++

# Flags
CFLAGS = -Wall -g -I./headers -I./source -I/usr/include/libcamera \
		 -I/usr/include/opencv4 -D USE_BCM2835_LIB
CXXFLAGS = -Wall -g -I/usr/include/libcamera -I/usr/include/opencv4 -std=c++17 -O2

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
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Compile C files
$(OUTDIR)/%.o: source/%.c
	@mkdir -p $(OUTDIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTDIR)/%.o: libraries/%.c
	@mkdir -p $(OUTDIR)
	$(CC) -c $< -o $@ $(CFLAGS)

# Run target
run: $(TARGET)
	sudo ./$(TARGET)

# Clean target
clean:
	rm -rf $(TARGET) $(OUTDIR)

.PHONY: all clean
