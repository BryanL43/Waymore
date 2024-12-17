# Compilers
CC = gcc
CXX = g++

# Flags for Debug and Release Builds

# -I/usr/include/libcamera -I/usr/include/opencv4 -I/usr/include/libcamera -I/usr/include/opencv4
CFLAGS_DEBUG = -Wall -g -I./headers -I./source -D USE_BCM2835_LIB -O0
CXXFLAGS_DEBUG = -Wall -g -I./libraries -std=c++17 -O0

# -I/usr/include/libcamera -I/usr/include/opencv4 -I/usr/include/libcamera -I/usr/include/opencv4
CFLAGS_RELEASE = -Wall -I./headers -I./source -D USE_BCM2835_LIB -O2
CXXFLAGS_RELEASE = -Wall -I./libraries -std=c++17 -O2

CFLAGS = $(CFLAGS_DEBUG) # Change to $(CFLAGS_RELEASE) for Release
CXXFLAGS = $(CXXFLAGS_DEBUG) # Change to $(CXXFLAGS_RELEASE) for Release

# Libraries (-lcamera -lcamera-base -lopencv_imgproc -lopencv_core -lopencv_imgcodecs removed)
LIBS = -lstdc++ -lbcm2835 -lm -lsl_lidar_sdk
LIDAR_LIB = -L./libraries

# Directories
OUTDIR = out

# Source Files
CFILES := $(wildcard source/*.c)
CPPFILES := $(wildcard source/*.cpp)

# Object Files
COBJECTS := $(patsubst source/%.c,$(OUTDIR)/%.o,$(CFILES))
CPPOBJECTS := $(patsubst source/%.cpp,$(OUTDIR)/%.o,$(CPPFILES))

# Target executable
TARGET = waymore

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(COBJECTS) $(CPPOBJECTS)
	@echo Linking $(TARGET)...
	$(CXX) $(COBJECTS) $(CPPOBJECTS) -o $(TARGET) $(LIBS) $(LIDAR_LIB)

# Compile C++ files
$(OUTDIR)/%.o: source/%.cpp
	@mkdir -p $(OUTDIR)
	@echo Compiling $<...
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Compile C files
$(OUTDIR)/%.o: source/%.c
	@mkdir -p $(OUTDIR)
	@echo Compiling $<...
	$(CC) -c $< -o $@ $(CFLAGS)

# Generate dependency files for automatic tracking
DEPFILES = $(COBJECTS:.o=.d) $(CPPOBJECTS:.o=.d)

%.d: %.c
	@$(CC) $(CFLAGS) -MM $< -MT $(@:.d=.o) > $@

%.d: %.cpp
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(@:.d=.o) > $@

-include $(DEPFILES)

# Clean target
clean:
	@echo Cleaning...
	rm -rf $(TARGET) $(OUTDIR) $(DEPFILES)

# Unity Build (Optional)
UNIFIED_CPP = $(OUTDIR)/unified.cpp
UNIFIED_OBJECT = $(OUTDIR)/unified.o

unity: $(UNIFIED_OBJECT)
	@echo Linking $(TARGET) with Unity Build...
	$(CXX) $< -o $(TARGET) $(LIBS) $(LIDAR_LIB)

$(UNIFIED_CPP): $(CPPFILES)
	@mkdir -p $(OUTDIR)
	@echo Creating unified source file...
	cat $(CPPFILES) > $@

$(UNIFIED_OBJECT): $(UNIFIED_CPP)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

run: $(TARGET)
	sudo ./$(TARGET)

.PHONY: all clean unity
