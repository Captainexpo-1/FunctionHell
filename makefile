# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++11

# Source files and object files

SRCS = ../main.cpp ./lexer/lexer.cpp ./lexer/tokens/tokens.cpp ./errors/error.cpp ./parser/ast/ASTNodes.cpp ./parser/parser.cpp ./backend/transpiler/transpiler.cpp
OBJS = $(addprefix $(OUTPUT_DIR)/obj/,$(SRCS:.cpp=.o))
PROGRAM_NAME = fh
# Target executable
SRC_DIR = src
OUTPUT_DIR = bin
TARGET = $(OUTPUT_DIR)/$(PROGRAM_NAME)

# Default target
all: $(TARGET)

# Compile source files into object files
$(OUTPUT_DIR)/obj/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@



# Clean up object files and the target executable
clean:
	rm -rf $(OBJS) $(OUTPUT_DIR)/

reset:
# Create output directory
	mkdir -p $(OUTPUT_DIR)/obj
# Create output sub-directories (there's probably a more concise way to do this)
	mkdir -p $(OUTPUT_DIR)/obj/lexer
	mkdir -p $(OUTPUT_DIR)/obj/lexer/tokens
	mkdir -p $(OUTPUT_DIR)/obj/errors
	mkdir -p $(OUTPUT_DIR)/obj/parser
	mkdir -p $(OUTPUT_DIR)/obj/parser/ast
	mkdir -p $(OUTPUT_DIR)/obj/backend
	mkdir -p $(OUTPUT_DIR)/obj/backend/transpiler





.PHONY: all clean