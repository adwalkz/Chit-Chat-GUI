PROG = ChatBox

CC      = gcc
LIBS    = `pkg-config --cflags --libs gtk+-3.0`
CFLAGS  = -std=c99 -Wall -Wextra

$(PROG): $(PROG).c
		$(CC) $(PROG).c -o $(PROG) $(CFLAGS) $(LIBS) -pthread

