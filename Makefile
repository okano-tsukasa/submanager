CC = gcc
CFLAGS = -Wall -Wextra -g -I/usr/include/postgresql
LDFLAGS = -lpq

SRCDIR = src
SRCS = $(SRCDIR)/main.c $(SRCDIR)/db.c $(SRCDIR)/ui.c
OBJS = $(SRCS:.c=.o)
TARGET = submanager

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

initdb:
	psql -d submanager -f sql/init.sql

.PHONY: all clean run initdb
