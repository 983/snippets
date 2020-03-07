CFLAGS += -Wall -Wextra -pedantic -Werror -g

all:
	gcc fs.c $(CFLAGS) -o fs `pkg-config fuse --cflags --libs`
