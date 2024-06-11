# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++11

# Source files and object files
SRCS = main.cpp lexer.cpp tokens.cpp ASTNodes.cpp parser.cpp
OBJS = $(addprefix $(OUTPUT_DIR)/obj/,$(SRCS:.cpp=.o))

# Target executable
SRC_DIR = src
OUTPUT_DIR = bin
TARGET = $(OUTPUT_DIR)/prgm

# Default target
all: $(TARGET)

# Compile source files into object files
$(OUTPUT_DIR)/obj/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Create output directory
$(shell mkdir -p $(OUTPUT_DIR)/obj)

# Clean up object files and the target executable
clean:
	rm -rf $(OBJS) $(OUTPUT_DIR)/

.PHONY: all clean