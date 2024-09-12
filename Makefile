# Makefile

# Compiler and flags
CC = gcc

# Libraries to link with
LDFLAGS = -lpapi -O3

# Target executable
TARGET = main

# Source files
SRCS = main.c

# Output file
OUTPUT = output.txt

# Default target
all: $(TARGET)
	./$(TARGET) > $(OUTPUT)

# Compile the program with PAPI
$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Remove the output file and target binary
clean:
	rm -f $(OUTPUT)
	rm -f $(TARGET)

# Clean and rebuild target
rebuild: clean all
