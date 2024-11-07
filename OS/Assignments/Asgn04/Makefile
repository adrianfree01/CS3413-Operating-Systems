# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I.

# Source files
SRCS = asgn04.c functions.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = asgn04

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Compile source files into object files
%.o: %.c definitions.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

