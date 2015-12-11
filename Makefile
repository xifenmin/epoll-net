CC      =   gcc
CFLAGS  += -g -Wall -D_x86
TARGET  = ./test_server
SOURCES = $(wildcard *.c)
OBJS    = $(patsubst %.c,%.o,$(SOURCES))

MYLIBS   = -I.

all:test_server

%c.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(MYLIBS) -lpthread

	chmod a+x $(TARGET)

.PHONY: clean rf
clean:
	rm -rf *.o $(TARGET)

rf:all


