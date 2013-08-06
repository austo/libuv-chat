UVSRC=/hd/_devResources/libuv/src
SRCDIR=src

CC = gcc

CFLAGS = -I/usr/local/include -I$(UVSRC) \
	-Wall -Wextra -Wno-unused-parameter
LDFLAGS = -L/usr/local/lib -lm -luv

ifeq ($(shell uname),Darwin)
LDFLAGS += -framework CoreServices
endif

ifeq ($(shell uname),Linux)
LDFLAGS += -lrt
endif

SRCS = $(SRCDIR)/experiment.c

OBJS = $(SRCS:.c=.o)

TARGET = wchat

.PHONY:	all clean

all:	wchat

clean:
	rm -f core $(TARGET) *.o

$(notdir $(OBJS)):	$(SRCS)
	$(CC) -c -g $(CFLAGS) $(SRCS)

wchat:	$(notdir $(OBJS))
	$(CC) $^ -o $@ $(LDFLAGS)
