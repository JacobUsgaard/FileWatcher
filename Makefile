CC=gcc
CC_FLAGS=-Wall

all: file_watcher

file_watcher: file_watcher.c
	$(CC) $(CC_FLAGS) file_watcher.c -o file_watcher

error_handler.o: error_handler.c
	$(CC) $(CC_FLAGS) error_handler.c -c -o error_handler.o

clean:
	rm -f file_watcher *.o
