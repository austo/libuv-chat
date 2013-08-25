SOURCE_DIR=$(DVR)/libuv-chat/src

CC = gcc

CFLAGS = -I/usr/local/include -Wall -Wextra -Wno-unused-parameter

LDFLAGS = -lm -luv

SRCS = $(SOURCE_DIR)/server.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all:    server	

clean:	
	rm -f server chat-server *.o core

$(notdir $(OBJS)):	$(SRCS) $(HDRS)
	$(CC) -c -g $(CFLAGS) $(SRCS)

chat-server:    main.o
	$(CC) $^ -o $@ $(LDFLAGS)

server:     server.o
	$(CC) $^ -o $@ $(LDFLAGS)
