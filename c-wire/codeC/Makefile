CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
EXEC = process_data
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJECTS)
		$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		rm -f $(EXEC) $(OBJECTS)

.PHONY: all clean