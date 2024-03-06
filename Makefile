# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDES = -Iheaders
TARGET = bin/jsh
SRC = src/jsh.c
OBJS = $(SRC:.c=.o)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Rule to clean the build
clean:
	rm -f $(TARGET) $(OBJS)
