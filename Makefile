.PHONY: all
PROJ = hxdmp

CC = clang -std=c17 -v -g
CFLAGS = -Wall -Wextra -pedantic -c -g \
		 -Wno-declaration-after-statement \
		 -Wno-c++98-compat \
		 -Wno-c++98-compat-pedantic \
		 -Wno-c++98-c++11-compat-pedantic \
		 -Wno-c23-compat \
		 -Wno-reserved-identifier \
		 -Wno-missing-braces \
		 -Wno-sizeof-pointer-div \
		 -Wno-ignored-pragmas \

# ------------------------------------------------------------------------------
# linking

# include dir
CFLAGS += -Iinclude/
CFLAGS += -Isrc/

# libs
LIBS   = 
LFLAGS = 

# ------------------------------------------------------------------------------
# building

BUILD_DIR = bin
SRC_DIR = src
EXEC = $(PROJ).exe

SOURCES := $(wildcard $(SRC_DIR)/*.c)

OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

REMOVE_CMD := rm -rf

all: build

build: dir $(BUILD_DIR)/$(EXEC)

dir:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(EXEC): $(OBJ)
	$(CC) $^ -o $@ $(LFLAGS)

$(OBJ): $(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

run: 
	./$(BUILD_DIR)/$(EXEC)

clean:
	$(REMOVE_CMD) $(BUILD_DIR)

compiledb:
	compiledb -n make all
