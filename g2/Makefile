
# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
#  -O3 enables aggressive compiler optimization
#  -std=c++11 enables c++11 syntax (auto, templates, ect)
CFLAGS  = -Wall -O3 -std=c++11
# define any directories containing header files other than /usr/include
INCLUDES = -I tclap-1.2.1/include/
# the build target executable:
TARGET = g2function

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET)